// tests/beman/execution/exec-task.test.cpp               -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <test/execution.hpp>
#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
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
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution;
import beman.execution.detail.simple_allocator;
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

#ifdef __cpp_exceptions
auto throwing_task() -> test_std::task<int> {
    throw std::runtime_error("task failure");
    co_return 0;
}
#endif

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

// the coroutine allocator type is unrelated to the task's allocator_type (std::allocator<std::byte>)
auto foreign_allocator_task(std::allocator_arg_t, std::pmr::polymorphic_allocator<>) -> test_std::task<int> {
    co_return 67;
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
    static_assert(std::same_as<test_std::completion_signatures_of_t<int_task>,
                               test_std::completion_signatures<test_std::set_value_t(int),
                                                               test_std::set_stopped_t(),
                                                               test_std::set_error_t(std::exception_ptr)>>);
    static_assert(std::same_as<test_std::completion_signatures_of_t<void_task>,
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

#ifdef __cpp_exceptions
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
        auto t = integer_error_task();
        static_assert(
            std::same_as<decltype(t)::error_types, test_std::completion_signatures<test_std::set_error_t(int)>>);
        test::use(test_std::sync_wait(std::move(t)));
    } catch (int value) {
        ASSERT(value == 17);
        caught = true;
    }
    ASSERT(caught);
}
#endif // __cpp_exceptions

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

auto test_task_allocator_arg_unrelated_to_allocator_type() -> void {
    counting_memory_resource          resource;
    std::pmr::polymorphic_allocator<> allocator{&resource};

    auto task = foreign_allocator_task(std::allocator_arg, allocator);
    ASSERT(resource.allocations == 1uz);
    ASSERT(resource.deallocations == 0uz);

    auto result = test_std::sync_wait(std::move(task));
    ASSERT(result.has_value());
    ASSERT(std::get<0>(*result) == 67);
    ASSERT(resource.deallocations == 1uz);
}

auto test_task_propagates_stop_token() -> void {
    test_std::inplace_stop_source source;
    ASSERT(source.request_stop());

    bool value{};
    auto operation = test_std::connect(stop_token_task(), stop_token_receiver{source.get_token(), &value});
    test_std::start(operation);
    ASSERT(value);
}

struct test_scheduler {
    using scheduler_concept = test_std::scheduler_tag;
    struct env {
        auto query(test_std::get_completion_scheduler_t<test_std::set_value_t>) const noexcept -> test_scheduler {
            return {this->alive};
        }
        ~env() { ASSERT(*alive); }
        bool* alive;
    };

    template <test_std::receiver Receiver>
    struct state {
        using operation_state_concept = test_std::operation_state_tag;
        std::remove_cvref_t<Receiver> receiver;
        bool*                         alive;
        auto                          start() & noexcept -> void { test_std::set_value(std::move(receiver)); }
        ~state() { ASSERT(*alive); }
    };
    struct sender {
        using sender_concept = test_std::sender_tag;

        using completion_signatures = test_std::completion_signatures<test_std::set_value_t()>;
        template <typename...>
        static consteval auto get_completion_signatures() noexcept -> completion_signatures {
            return {};
        }
        ~sender() { ASSERT(*alive); }
        auto get_env() const noexcept -> env { return {this->alive}; }
        template <test_std::receiver Receiver>
        auto connect(Receiver&& r) && noexcept {
            return state<Receiver>{std::forward<Receiver>(r), this->alive};
        }
        bool* alive;
    };

    ~test_scheduler() { ASSERT(*alive); }

    auto schedule() const noexcept -> sender { return {this->alive}; }
    auto operator==(const test_scheduler&) const noexcept -> bool = default;
    auto query(test_std::get_forward_progress_guarantee_t) const noexcept -> test_std::forward_progress_guarantee {
        return test_std::forward_progress_guarantee::concurrent;
    }

    bool* alive;
};
static_assert(test_std::sender<test_scheduler::sender>);
static_assert(test_std::sender_in<test_scheduler::sender>);
static_assert(test_std::scheduler<test_scheduler>);

template <typename T>
struct test_allocator {
    using value_type = T;
    test_allocator(bool* alive) : alive(alive) {}
    template <typename U>
    test_allocator(const test_allocator<U>& other) : alive(other.alive) {}
    ~test_allocator() { ASSERT(*alive); }

    auto allocate(std::size_t n) -> value_type* {
        return static_cast<value_type*>(::operator new(n * sizeof(value_type)));
    }
    auto deallocate(value_type* p, std::size_t) -> void { ::operator delete(p); }

    auto operator==(const test_allocator&) const noexcept -> bool = default;

    bool* alive;
};
static_assert(test_detail::simple_allocator<test_allocator<int>>);

struct test_stop_token {
    template <typename Callback>
    struct callback_type {
        Callback callback;
        callback_type(test_stop_token token, Callback c) : callback(::std::move(c)), alive(token.alive) {}
        ~callback_type() { ASSERT(*alive); }
        bool* alive;
    };
    ~test_stop_token() { ASSERT(*alive); }
    auto  stop_requested() const noexcept -> bool { return false; }
    auto  stop_possible() const noexcept -> bool { return true; }
    auto  operator==(const test_stop_token&) const noexcept -> bool = default;
    bool* alive;
};
static_assert(test_std::stoppable_token<test_stop_token>);

auto test_scheduler_lifetime() -> void {
    struct receiver {
        bool* alive;
        using receiver_concept = test_std::receiver_tag;
        auto get_env() const noexcept -> receiver { return {this->alive}; }
        auto query(test_std::get_stop_token_t) const noexcept -> test_stop_token { return {this->alive}; }
        auto query(test_std::get_start_scheduler_t) const noexcept -> test_scheduler { return {this->alive}; }
        auto query(test_std::get_scheduler_allocator_t) const noexcept -> test_allocator<std::byte> {
            return {this->alive};
        }

        auto set_value() && noexcept -> void { *alive = false; }
        auto set_error(std::exception_ptr) && noexcept -> void { *alive = false; }
        auto set_stopped() && noexcept -> void { *alive = false; }
    };
    static_assert(test_std::receiver<receiver>);

    {
        bool alive{true};
        auto state{test_std::connect([]() -> test_std::task<> { co_return; }(), receiver{&alive})};
        test_std::start(state);
    }
    {
        bool alive{true};
        auto state{
            test_std::connect([]() -> test_std::task<> { co_await test_std::just_error(42); }(), receiver{&alive})};
        test_std::start(state);
    }
    {
        bool alive{true};
        auto state{
            test_std::connect([]() -> test_std::task<> { co_await test_std::just_stopped(); }(), receiver{&alive})};
        test_std::start(state);
    }
    {
        bool alive{true};
        auto state{test_std::connect(
            []() -> test_std::task<> {
                [[maybe_unused]] auto token{co_await test_std::read_env(test_std::get_stop_token)};
            }(),
            receiver{&alive})};
        test_std::start(state);
    }
}
} // namespace

TEST(exec_task) {
    test_task_interface();
    test_task_is_lazy_and_completes_with_values();
    test_task_awaits_senders_and_tasks();
    test_task_stopped_completion();
#ifdef __cpp_exceptions
    test_task_errors();
#endif
    test_task_uses_pmr_allocator_arg();
    test_task_allocator_arg_unrelated_to_allocator_type();
    test_task_propagates_stop_token();
    test_scheduler_lifetime();
}
