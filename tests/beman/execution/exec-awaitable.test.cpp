// src/beman/execution/tests/exec-awaitable.test.cpp                -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution/detail/await_suspend_result.hpp>
#include <beman/execution/detail/is_awaiter.hpp>
#include <beman/execution/detail/is_awaitable.hpp>
#include <beman/execution/detail/get_awaiter.hpp>
#include <beman/execution/detail/await_result_type.hpp>
#include <beman/execution/detail/has_as_awaitable.hpp>
#include <beman/execution/detail/with_await_transform.hpp>
#include <beman/execution/detail/env_promise.hpp>
#include <test/execution.hpp>
#endif
#include <coroutine>
#include <concepts>

// ----------------------------------------------------------------------------

namespace {
struct type {};

template <typename Ready, typename Suspend, typename... Resume>
struct awaiter {
    int  value{};
    auto await_ready() -> Ready { return {}; }
    auto await_suspend(auto) -> Suspend { return {}; }
    auto await_resume(Resume...) -> void {}
};

template <typename T>
struct typed_awaiter {
    auto await_ready() -> bool { return {}; }
    auto await_suspend() -> bool { return {}; }
    auto await_resume() -> T { return {}; }
};

template <typename Promise>
struct awaiter_with_handle {
    auto await_ready() -> bool { return {}; }
    auto await_suspend(std::coroutine_handle<Promise>) -> bool { return {}; }
    auto await_resume() -> void {}
};

struct co_awaiter {
    int value{};
};
auto operator co_await(co_awaiter obj) -> awaiter<bool, bool> { return {3 + obj.value}; }

struct mem_co_awaiter {
    int  value{};
    auto operator co_await() -> awaiter<bool, bool> { return {5 + this->value}; }
};
// auto operator co_await(mem_co_awaiter obj) -> awaiter<bool, bool> { return { 3 + obj.value }; }

template <typename T, bool Noexcept = true>
struct with_as_awaitable {
    auto as_awaitable(auto&&) noexcept(Noexcept) -> T { return {}; }
};

struct promise_type {
    auto await_transform(awaiter<bool, bool> a) { return awaiter<bool, std::coroutine_handle<>>{2 + a.value}; }

    auto await_transform(awaiter<bool, int> a) { return co_awaiter{4 + a.value}; }
    auto await_transform(awaiter<bool, long> a) { return mem_co_awaiter{5 + a.value}; }
    auto await_transform(typed_awaiter<bool>) { return mem_co_awaiter{5}; }
};

auto test_await_suspend_result() -> void {
    static_assert(test_detail::await_suspend_result<void>);
    static_assert(test_detail::await_suspend_result<bool>);
    static_assert(test_detail::await_suspend_result<std::coroutine_handle<>>);
    static_assert(test_detail::await_suspend_result<std::coroutine_handle<promise_type>>);
    static_assert(not test_detail::await_suspend_result<int>);
}

auto test_is_awaiter() -> void {
    static_assert(not test_detail::is_awaiter<int, promise_type>);
    static_assert(test_detail::is_awaiter<awaiter<bool, void>, promise_type>);
    static_assert(test_detail::is_awaiter<awaiter<bool, bool>, promise_type>);
    static_assert(test_detail::is_awaiter<awaiter<bool, std::coroutine_handle<>>, promise_type>);
    static_assert(test_detail::is_awaiter<awaiter<bool, std::coroutine_handle<promise_type>>, promise_type>);
    static_assert(not test_detail::is_awaiter<awaiter<type, void>, promise_type>);
    static_assert(not test_detail::is_awaiter<awaiter<bool, type>, promise_type>);
    static_assert(not test_detail::is_awaiter<awaiter<bool, void, int>, promise_type>);
    static_assert(test_detail::is_awaiter<awaiter_with_handle<promise_type>, promise_type>);
    static_assert(test_detail::is_awaiter<awaiter_with_handle<void>, promise_type>);
    static_assert(not test_detail::is_awaiter<awaiter_with_handle<type>, promise_type>);
}

auto test_get_awaiter() -> void {
    promise_type promise{};

    auto a1{test_detail::get_awaiter(awaiter<bool, void>{17}, promise)};
    static_assert(std::same_as<awaiter<bool, void>, decltype(a1)>);
    ASSERT(a1.value == 17);

    auto a2{test_detail::get_awaiter(awaiter<bool, bool>{17}, promise)};
    static_assert(std::same_as<awaiter<bool, std::coroutine_handle<>>, decltype(a2)>);
    ASSERT(a2.value == 19);

    auto a3{test_detail::get_awaiter(co_awaiter{17}, promise)};
    static_assert(std::same_as<awaiter<bool, bool>, decltype(a3)>);
    ASSERT(a3.value == 20);

    auto a4{test_detail::get_awaiter(awaiter<bool, int>{17}, promise)};
    static_assert(std::same_as<awaiter<bool, bool>, decltype(a4)>);
    ASSERT(a4.value == 24);

    auto a5{test_detail::get_awaiter(awaiter<bool, long>{17}, promise)};
    static_assert(std::same_as<awaiter<bool, bool>, decltype(a5)>);
    ASSERT(a5.value == 27);

    auto a6{test_detail::get_awaiter(mem_co_awaiter{17}, promise)};
    static_assert(std::same_as<awaiter<bool, bool>, decltype(a6)>);
    ASSERT(a6.value == 22);
}

auto test_is_awaitable() -> void {
    static_assert(test_detail::is_awaitable<awaiter<bool, void>, promise_type>);
    static_assert(test_detail::is_awaitable<awaiter<bool, bool>, promise_type>);
    static_assert(test_detail::is_awaitable<co_awaiter, promise_type>);
    static_assert(test_detail::is_awaitable<mem_co_awaiter, promise_type>);
    static_assert(not test_detail::is_awaitable<awaiter<bool, double>, promise_type>);
}

auto test_await_result_type() -> void {
    static_assert(std::same_as<int, test_detail::await_result_type<typed_awaiter<int>, promise_type>>);
    static_assert(std::same_as<void, test_detail::await_result_type<typed_awaiter<bool>, promise_type>>);
}

auto test_has_as_awaitable() -> void {
    static_assert(not test_detail::has_as_awaitable<int, promise_type>);
    static_assert(not test_detail::has_as_awaitable<with_as_awaitable<int>, promise_type>);
    static_assert(test_detail::has_as_awaitable<with_as_awaitable<awaiter<bool, bool>>, promise_type>);
    static_assert(test_detail::has_as_awaitable<with_as_awaitable<co_awaiter>, promise_type>);
}

struct promise_with_await_transform : test_detail::with_await_transform<promise_with_await_transform> {};

auto test_with_await_transform() -> void {
    promise_with_await_transform promise{};

    static_assert(noexcept(promise.await_transform(awaiter<bool, void>{})));
    auto a1{promise.await_transform(awaiter<bool, void>{})};
    static_assert(std::same_as<awaiter<bool, void>, decltype(a1)>);

    static_assert(noexcept(promise.await_transform(with_as_awaitable<awaiter<bool, void>, true>{})));
    static_assert(not noexcept(promise.await_transform(with_as_awaitable<awaiter<bool, void>, false>{})));
    auto a2{promise.await_transform(with_as_awaitable<awaiter<bool, void>>{})};
    test::use(a2);
    static_assert(std::same_as<awaiter<bool, void>, decltype(a2)>);
}

auto test_env_promise() -> void {
    struct local_env {};

    test_detail::env_promise<local_env> promise;

    static_assert(noexcept(promise.get_return_object()));
    static_assert(noexcept(promise.initial_suspend()));
    static_assert(noexcept(promise.final_suspend()));
    static_assert(noexcept(promise.unhandled_exception()));
    static_assert(std::same_as<void, decltype(promise.unhandled_exception())>);
    static_assert(noexcept(promise.return_void()));
    static_assert(std::same_as<void, decltype(promise.return_void())>);
    static_assert(noexcept(promise.unhandled_stopped()));
    static_assert(std::same_as<std::coroutine_handle<>, decltype(promise.unhandled_stopped())>);
    static_assert(noexcept(std::as_const(promise).get_env()));
    static_assert(std::same_as<const local_env&, decltype(promise.get_env())>);
}
} // namespace

TEST(exec_awaitable) {
    test_await_suspend_result();
    test_is_awaiter();
    test_get_awaiter();
    test_is_awaitable();
    test_await_result_type();
    test_has_as_awaitable();
    test_with_await_transform();
    test_env_promise();
}
