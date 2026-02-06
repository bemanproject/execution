// src/beman/execution/tests/exec-then.test.cpp                     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <concepts>
#include <memory_resource>
#include <optional>
#include <tuple>
#include <utility>
#include <test/execution.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution/detail/then.hpp>
#include <beman/execution/detail/completion_signatures_of_t.hpp>
#include <beman/execution/detail/connect.hpp>
#include <beman/execution/detail/forwarding_query.hpp>
#include <beman/execution/detail/read_env.hpp>
#include <beman/execution/detail/just.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/sender_in.hpp>
#include <beman/execution/detail/start.hpp>
#include <beman/execution/detail/sync_wait.hpp>
#endif

// ----------------------------------------------------------------------------

namespace {
struct error {
    int value{};
};
struct non_sender {};

struct get_value_t : test_std::forwarding_query_t {
    auto operator()(const auto& e) const noexcept -> int { return e.query(*this); }
};

struct receiver {
    using receiver_concept = test_std::receiver_t;

    auto set_error(auto&&) && noexcept -> void {}
    auto set_stopped() && noexcept -> void {}
    auto set_value(auto&&...) && noexcept -> void {}

    struct env {
        auto query(const get_value_t&) const noexcept { return 42; }
    };
    auto get_env() const noexcept { return env{}; }
};

template <typename... T>
struct sender {
    using sender_concept        = test_std::sender_t;
    using completion_signatures = test_std::completion_signatures<T...>;
};

template <bool Expect>
auto test_has(auto cpo, auto in_sender, auto fun) -> void {
    static_assert(test_std::receiver<receiver>);
    static_assert(Expect == requires {
        { cpo(in_sender, fun) } -> test_std::sender;
    });
    if constexpr (Expect) {
        static_assert(requires { cpo(fun); });
        static_assert(requires { in_sender | cpo(fun); });
        static_assert(requires {
            { in_sender | cpo(fun) } -> test_std::sender;
        });
#ifndef _MSC_VER
        //-dk:TODO re-enable this test
        static_assert(requires {
            {
                in_sender | cpo(fun) | cpo([](auto&&...) {})
            } -> test_std::sender;
        });
#endif
        auto sender{cpo(in_sender, fun)};
        auto op{test_std::connect(::std::move(sender), receiver{})};
        test_std::start(op);

        const auto csender{cpo(in_sender, fun)};
        auto       cop{test_std::connect(std::move(csender), receiver{})};
        test_std::start(cop);
    }
}

template <typename... Completions>
auto test_then_type(auto sender) {
    static_assert(std::same_as<test_std::completion_signatures<Completions...>,
                               test_std::completion_signatures_of_t<decltype(sender), test_std::env<>>>);
}

auto test_then_type() -> void {
    test_then_type<test_std::set_value_t()>(test_std::just(0) | test_std::then([](auto) noexcept {}));
    test_then_type<test_std::set_value_t(int)>(test_std::just() | test_std::then([]() noexcept { return 0; }));
    test_then_type<test_std::set_error_t(int)>(test_std::just_error(0) | test_std::then([]() noexcept { return 0; }));
    test_then_type<test_std::set_stopped_t()>(test_std::just_stopped() | test_std::then([]() noexcept { return 0; }));

    test_then_type<test_std::set_value_t()>(test_std::just() | test_std::upon_error([]() noexcept { return 0; }));
    test_then_type<test_std::set_value_t(int)>(test_std::just_error(error{}) |
                                               test_std::upon_error([](error) noexcept { return 0; }));
    test_then_type<test_std::set_stopped_t()>(test_std::just_stopped() | test_std::upon_error([]() noexcept {}));

    test_then_type<test_std::set_value_t(int)>(test_std::just(0) | test_std::upon_stopped([]() noexcept {}));
    test_then_type<test_std::set_error_t(int)>(test_std::just_error(0) | test_std::upon_stopped([]() noexcept {}));
    test_then_type<test_std::set_value_t()>(test_std::just_stopped() | test_std::upon_stopped([]() noexcept {}));
}

auto test_then_multi_type() -> void {
    static_assert(test_std::sender<sender<>>);
    static_assert(test_std::sender_in<sender<>>);

    test_then_type<test_std::set_value_t(),
                   test_std::set_value_t(int, int),
                   test_std::set_error_t(error),
                   test_std::set_stopped_t()>(sender<test_std::set_value_t(),
                                                     test_std::set_value_t(int, int),
                                                     test_std::set_error_t(error),
                                                     test_std::set_stopped_t()>());
    test_then_type<test_std::set_value_t(bool),
                   test_std::set_error_t(error),
                   test_std::set_stopped_t(),
                   test_std::set_error_t(::std::exception_ptr)>(sender<test_std::set_value_t(),
                                                                       test_std::set_value_t(int, int),
                                                                       test_std::set_error_t(error),
                                                                       test_std::set_stopped_t()>() |
                                                                test_std::then([](auto&&...) { return true; }));
    test_then_type<test_std::set_value_t(),
                   test_std::set_value_t(int, int),
                   test_std::set_value_t(bool),
                   test_std::set_stopped_t(),
                   test_std::set_error_t(::std::exception_ptr)>(sender<test_std::set_value_t(),
                                                                       test_std::set_value_t(int, int),
                                                                       test_std::set_error_t(error),
                                                                       test_std::set_stopped_t()>() |
                                                                test_std::upon_error([](error) { return true; }));
    test_then_type<test_std::set_value_t(),
                   test_std::set_value_t(int, int),
                   test_std::set_stopped_t(),
                   test_std::set_error_t(::std::exception_ptr)>(sender<test_std::set_value_t(),
                                                                       test_std::set_value_t(int, int),
                                                                       test_std::set_error_t(error),
                                                                       test_std::set_stopped_t()>() |
                                                                test_std::upon_error([](error) {}));
    test_then_type<test_std::set_value_t(int),
                   test_std::set_value_t(int, int),
                   test_std::set_error_t(error),
                   test_std::set_value_t()>(sender<test_std::set_value_t(int),
                                                   test_std::set_value_t(int, int),
                                                   test_std::set_error_t(error),
                                                   test_std::set_stopped_t()>() |
                                            test_std::upon_stopped([]() noexcept {}));
    test_then_type<test_std::set_value_t(), test_std::set_value_t(int, int), test_std::set_error_t(error)>(
        sender<test_std::set_value_t(),
               test_std::set_value_t(int, int),
               test_std::set_error_t(error),
               test_std::set_stopped_t()>() |
        test_std::upon_stopped([]() noexcept {}));
    test_then_type<test_std::set_value_t(),
                   test_std::set_value_t(int, int),
                   test_std::set_error_t(error),
                   test_std::set_error_t(::std::exception_ptr)>(sender<test_std::set_value_t(),
                                                                       test_std::set_value_t(int, int),
                                                                       test_std::set_error_t(error),
                                                                       test_std::set_stopped_t()>() |
                                                                test_std::upon_stopped([] {}));
}

auto test_then_value() -> void {
    ASSERT(
        std::tuple{17} ==
        test_std::sync_wait(test_std::just(5, 12) | test_std::then([](int a, int b) { return a + b; })).value_or(0));
    ASSERT(std::tuple{17} ==
           test_std::sync_wait(test_std::just_error(17) | test_std::upon_error([](int a) { return a; })));
    ASSERT(std::tuple{17} ==
           test_std::sync_wait(test_std::just_stopped() | test_std::upon_stopped([]() { return 17; })));
}

struct memory_env {
    std::pmr::polymorphic_allocator<> allocator;
    auto                              query(const test_std::get_allocator_t&) const noexcept { return allocator; }
};
struct memory_receiver {
    using receiver_concept = test_std::receiver_t;
    std::pmr::polymorphic_allocator<> allocator;
    auto                              get_env() const noexcept { return memory_env{this->allocator}; }

    auto set_error(auto&&) && noexcept -> void {}
    auto set_stopped() && noexcept -> void {}
    auto set_value(auto&&...) && noexcept -> void {}
};
struct counting_resource : std::pmr::memory_resource {
    std::size_t count{};
    auto        do_allocate(std::size_t size, std::size_t) -> void* override {
        ++this->count;
        return operator new(size);
    }
    auto do_deallocate(void* p, std::size_t, std::size_t) -> void override { operator delete(p); }
    auto do_is_equal(const std::pmr::memory_resource& other) const noexcept -> bool override { return this == &other; }
};

struct allocator_fun {
    using allocator_type = std::pmr::polymorphic_allocator<>;

    std::pmr::polymorphic_allocator<> alloc;
    std::byte*                        data{nullptr};

    explicit allocator_fun(std::pmr::polymorphic_allocator<> all) : alloc(all), data(alloc.allocate(1)) {}
    allocator_fun(const allocator_fun&, std::pmr::polymorphic_allocator<> = {}) {}
    allocator_fun(allocator_fun&& other) noexcept : alloc(other.alloc), data(std::exchange(other.data, nullptr)) {}
    allocator_fun(allocator_fun&& other, std::pmr::polymorphic_allocator<> all)
        : alloc(all), data(alloc == other.alloc ? std::exchange(other.data, nullptr) : alloc.allocate(1)) {}
    ~allocator_fun() {
        if (this->data)
            this->alloc.deallocate(this->data, 1u);
    }
    auto operator=(const allocator_fun&) -> allocator_fun = delete;
    auto operator=(allocator_fun&&) -> allocator_fun      = delete;

    auto operator()(auto&&...) const {}
};

auto test_then_allocator() -> void {
    static_assert(std::uses_allocator_v<allocator_fun, std::pmr::polymorphic_allocator<>>);
    counting_resource resource1{};
    ASSERT(resource1.count == 0u);
    auto sender{test_std::just() | test_std::then(allocator_fun(&resource1))};
    ASSERT(resource1.count == 1u);

    counting_resource resource2{};
    ASSERT(resource2.count == 0u);
    auto state{test_std::connect(std::move(sender), memory_receiver{&resource2})};
    ASSERT(resource2.count == 1u);
}

auto test_then_env() -> void {
    {
        receiver r{};
        auto     e{test_std::get_env(r)};
        ASSERT(e.query(get_value_t{}) == 42);
        ASSERT(get_value_t{}(e) == 42);
    }
    {
        auto state{test_std::connect(test_std::read_env(get_value_t{}), receiver{})};
        test_std::start(state);
    }
    {
        int  value{0};
        auto state{test_std::connect(test_std::then(test_std::read_env(get_value_t{}), [&value](int v) { value = v; }),
                                     receiver{})};
        ASSERT(value == 0);
        test_std::start(state);
        ASSERT(value == 42);
    }
}

} // namespace

TEST(exec_then) {
    static_assert(std::same_as<const test_std::then_t, decltype(test_std::then)>);
    static_assert(std::same_as<const test_std::upon_error_t, decltype(test_std::upon_error)>);
    static_assert(std::same_as<const test_std::upon_stopped_t, decltype(test_std::upon_stopped)>);

    test_has<false>(test_std::then, non_sender{}, [] {});
    test_has<true>(test_std::then, test_std::just(), [] {});
    test_has<true>(test_std::then, test_std::just_error(error{3}), [] {});
    test_has<true>(test_std::then, test_std::just_stopped(), [] {});

    test_then_type();
    test_then_multi_type();
    test_then_value();
    test_then_allocator();

    test_then_env();
}
