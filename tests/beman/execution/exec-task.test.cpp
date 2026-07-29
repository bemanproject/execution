// tests/beman/execution/exec-task.test.cpp               -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <concepts>
#include <coroutine>
#include <exception>
#include <memory>
#include <memory_resource>
#include <new>
#include <stdexcept>
#include <thread>
#include <tuple>
#include <type_traits>
#include <utility>
#include <test/execution.hpp>

#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution/execution.hpp>
#endif

// ----------------------------------------------------------------------------

namespace {

auto value_task(bool& started) -> test_std::task<int> {
    started = true;
    co_return 42;
}

auto void_task(bool& completed) -> test_std::task<> {
    completed = true;
    co_return;
}

auto sender_awaiting_task() -> test_std::task<int> {
    const auto value = co_await test_std::just(21);
    co_return 2 * value;
}

auto nested_task() -> test_std::task<int> { co_return co_await sender_awaiting_task(); }

auto stopped_task(bool& reached_after_stop) -> test_std::task<> {
    co_await test_std::just_stopped();
    reached_after_stop = true;
}

auto nested_stopped_task(bool& reached_after_stop) -> test_std::task<> {
    co_await stopped_task(reached_after_stop);
    reached_after_stop = true;
}

auto throwing_task() -> test_std::task<int> {
    throw std::runtime_error("task failure");
    co_return 0;
}

struct integer_error_environment {
    using error_types = test_std::completion_signatures<test_std::set_error_t(int)>;
};

auto integer_error_task() -> test_std::task<void, integer_error_environment> { co_yield test_std::with_error{17}; }

struct pmr_environment {
    using allocator_type = std::pmr::polymorphic_allocator<>;
};

struct counting_memory_resource final : std::pmr::memory_resource {
    std::size_t allocations{};
    std::size_t deallocations{};

    auto do_allocate(std::size_t bytes, std::size_t alignment) -> void* override {
        ++allocations;
        return ::operator new(bytes, std::align_val_t{alignment});
    }

    auto do_deallocate(void* pointer, std::size_t, std::size_t alignment) -> void override {
        ++deallocations;
        ::operator delete(pointer, std::align_val_t{alignment});
    }

    auto do_is_equal(const std::pmr::memory_resource& other) const noexcept -> bool override { return this == &other; }
};

auto pmr_allocator_task(std::allocator_arg_t, std::pmr::polymorphic_allocator<> alloc)
    -> test_std::task<int, pmr_environment> {
    auto frame_alloc = co_await test_std::read_env(test_std::get_allocator);
    static_assert(std::same_as<decltype(frame_alloc), std::pmr::polymorphic_allocator<>>);
    ASSERT(frame_alloc == alloc);
    co_return 42;
}

auto scheduler_task() -> test_std::task<int> {
    const auto scheduler = co_await test_std::read_env(test_std::get_scheduler);
    static_assert(std::same_as<std::remove_cvref_t<decltype(scheduler)>, test_std::task_scheduler>);
    co_await test_std::schedule(scheduler);
    co_return 42;
}

struct stop_token_env {
    test_std::inplace_stop_token token;

    auto query(const test_std::get_stop_token_t&) const noexcept -> test_std::inplace_stop_token { return token; }
    auto query(const test_std::get_scheduler_t&) const noexcept -> test_std::inline_scheduler { return {}; }
    auto query(const test_std::get_start_scheduler_t&) const noexcept -> test_std::inline_scheduler { return {}; }
};

struct stop_token_receiver {
    using receiver_concept = test_std::receiver_tag;

    test_std::inplace_stop_token token;
    bool*                        result;

    auto set_value(bool value) && noexcept -> void { *result = value; }
    auto set_error(auto&&) && noexcept -> void { ASSERT(nullptr == "task completed with an unexpected error"); }
    auto set_stopped() && noexcept -> void { ASSERT(nullptr == "task completed unexpectedly stopped"); }
    auto get_env() const noexcept -> stop_token_env { return {token}; }
};

auto stop_token_task() -> test_std::task<bool> {
    const auto token = co_await test_std::read_env(test_std::get_stop_token);
    co_return token.stop_requested();
}

auto test_task_interface() -> void {
    using int_task  = test_std::task<int>;
    using void_task = test_std::task<>;

    static_assert(test_std::sender<int_task>);
    static_assert(!std::default_initializable<int_task>);
    static_assert(!std::copy_constructible<int_task>);
    static_assert(std::movable<int_task>);
    static_assert(std::same_as<typename int_task::completion_signatures,
                               test_std::completion_signatures<test_std::set_value_t(int),
                                                               test_std::set_stopped_t(),
                                                               test_std::set_error_t(std::exception_ptr)>>);
    static_assert(std::same_as<typename void_task::completion_signatures,
                               test_std::completion_signatures<test_std::set_value_t(),
                                                               test_std::set_stopped_t(),
                                                               test_std::set_error_t(std::exception_ptr)>>);
}

auto test_task_is_lazy_and_completes_with_values() -> void {
    bool started{};
    auto task = value_task(started);
    ASSERT(!started);

    auto result = test_std::sync_wait(std::move(task));
    ASSERT(started);
    ASSERT(result.has_value());
    ASSERT(std::get<0>(*result) == 42);

    bool completed{};
    auto void_result = test_std::sync_wait(void_task(completed));
    ASSERT(completed);
    ASSERT(void_result.has_value());
}

auto test_task_awaits_senders_and_tasks() -> void {
    auto result = test_std::sync_wait(nested_task());
    ASSERT(result.has_value());
    ASSERT(std::get<0>(*result) == 42);

    auto scheduler_result = test_std::sync_wait(scheduler_task());
    ASSERT(scheduler_result.has_value());
    ASSERT(std::get<0>(*scheduler_result) == 42);
}

auto test_task_stopped_completion() -> void {
    bool reached_after_stop{};
    auto result = test_std::sync_wait(nested_stopped_task(reached_after_stop));
    ASSERT(!result.has_value());
    ASSERT(!reached_after_stop);
}

auto test_task_errors() -> void {
    bool caught{};
    try {
        test::use(test_std::sync_wait(throwing_task()));
    } catch (const std::runtime_error&) {
        caught = true;
    }
    ASSERT(caught);

    caught = false;
    try {
        test::use(test_std::sync_wait(integer_error_task()));
    } catch (int value) {
        ASSERT(value == 17);
        caught = true;
    }
    ASSERT(caught);
}

auto test_task_uses_pmr_allocator_arg() -> void {
    counting_memory_resource          resource;
    std::pmr::polymorphic_allocator<> allocator{&resource};

    auto task = pmr_allocator_task(std::allocator_arg, allocator) |
                test_std::write_env(test_std::prop{test_std::get_allocator, allocator});
    ASSERT(resource.allocations == 1uz);

    auto result = test_std::sync_wait(std::move(task));
    ASSERT(result.has_value());
    ASSERT(std::get<0>(*result) == 42);
    ASSERT(resource.deallocations == resource.allocations);
}

auto test_task_propagates_stop_token() -> void {
    test_std::inplace_stop_source source;
    ASSERT(source.request_stop());

    bool value{};
    auto operation = test_std::connect(stop_token_task(), stop_token_receiver{source.get_token(), &value});
    test_std::start(operation);
    ASSERT(value);
}

} // namespace

TEST(exec_task) {
    test_task_interface();
    test_task_is_lazy_and_completes_with_values();
    test_task_awaits_senders_and_tasks();
    test_task_stopped_completion();
    test_task_errors();
    test_task_uses_pmr_allocator_arg();
    test_task_propagates_stop_token();
}
