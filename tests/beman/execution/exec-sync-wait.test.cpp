// src/beman/execution/tests/exec-sync-wait.test.cpp                -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution/execution.hpp>
#endif
#include <beman/execution/detail/sync_wait.hpp>
#include <test/execution.hpp>

#include <exception>
#include <concepts>
#include <utility>

// ----------------------------------------------------------------------------

namespace {
auto use(auto&&...) {}

template <int>
struct arg {
    int  value{};
    auto operator==(const arg&) const -> bool = default;
};
struct error : std::exception {
    int value{};
    explicit error(int v) : value(v) {}
};
struct sender {
    using sender_concept = test_std::sender_t;
};

struct sender_in {
    using sender_concept        = test_std::sender_t;
    using completion_signatures = test_std::completion_signatures<test_std::set_value_t(bool, int),
                                                                  test_std::set_error_t(error),
                                                                  test_std::set_stopped_t()>;

    template <typename Receiver>
    struct state {
        using operation_state_concept = test_std::operation_state_t;

        std::remove_cvref_t<Receiver> receiver;

        auto start() & noexcept -> void {}
    };

    template <typename Receiver>
    auto connect(Receiver&& receiver) noexcept -> state<Receiver> {
        return {::std::forward<Receiver>(receiver)};
    }
};

struct send_error {
    using sender_concept        = test_std::sender_t;
    using completion_signatures = test_std::
        completion_signatures<test_std::set_value_t(), test_std::set_error_t(error), test_std::set_stopped_t()>;

    template <typename Receiver>
    struct state {
        using operation_state_concept = test_std::operation_state_t;

        std::remove_cvref_t<Receiver> receiver;
        int                           value;

        auto start() & noexcept -> void { test_std::set_error(std::move(this->receiver), error{this->value}); }
    };

    int value{};

    template <typename Receiver>
    auto connect(Receiver&& receiver) noexcept -> state<Receiver> {
        return {::std::forward<Receiver>(receiver), this->value};
    }
};

struct send_stopped {
    using sender_concept        = test_std::sender_t;
    using completion_signatures = test_std::
        completion_signatures<test_std::set_value_t(), test_std::set_error_t(error), test_std::set_stopped_t()>;

    template <typename Receiver>
    struct state {
        using operation_state_concept = test_std::operation_state_t;

        std::remove_cvref_t<Receiver> receiver;

        auto start() & noexcept -> void { test_std::set_stopped(std::move(this->receiver)); }
    };

    template <typename Receiver>
    auto connect(Receiver&& receiver) noexcept -> state<Receiver> {
        return {::std::forward<Receiver>(receiver)};
    }
};

template <bool Expect>
auto test_has_sync_wait(auto&& sender) -> void {
    static_assert(Expect == requires { test_std::sync_wait(sender); });
}

auto test_sync_wait_env() -> void {
    test_std::run_loop         rl{};
    test_detail::sync_wait_env env{&rl};
    ASSERT(env.loop == &rl);

    static_assert(requires {
        { test_std::get_scheduler(env) } noexcept -> test_std::scheduler;
    });
    static_assert(requires {
        { test_std::get_delegation_scheduler(env) } noexcept -> test_std::scheduler;
    });
    ASSERT(test_std::get_scheduler(env) == rl.get_scheduler());
    ASSERT(test_std::get_delegation_scheduler(env) == rl.get_scheduler());
}

auto test_sync_wait_result_type() -> void {
    arg<0>       arg0{};
    const arg<1> arg1{};
    static_assert(std::same_as<std::optional<std::tuple<arg<0>, arg<1>, arg<2>>>,
                               test_detail::sync_wait_result_type<decltype(test_std::just(arg0, arg1, arg<2>{}))>>);
}

auto test_sync_wait_state() -> void {
    using type = test_detail::sync_wait_state<decltype(test_std::just(arg<0>{}))>;
    static_assert(std::same_as<test_std::run_loop, decltype(type{}.loop)>);
    static_assert(std::same_as<std::exception_ptr, decltype(type{}.error)>);
    static_assert(std::same_as<std::optional<std::tuple<arg<0>>>, decltype(type{}.result)>);
}

auto test_sync_wait_receiver() -> void {
    {
        using local_sender = decltype(test_std::just(arg<0>{}, arg<1>{}, arg<2>{}));
        test_detail::sync_wait_state<local_sender> state{};
        ASSERT(not state.result);
        ASSERT(not state.error);
        test_std::set_value(test_detail::sync_wait_receiver<local_sender>{&state}, arg<0>{2}, arg<1>{3}, arg<2>{5});
        ASSERT(state.result);
        ASSERT(not state.error);
        ASSERT(*state.result == (std::tuple{arg<0>{2}, arg<1>{3}, arg<2>{5}}));
    }
    {
        using local_sender = decltype(test_std::just(arg<0>{}, arg<1>{}, arg<2>{}));
        test_detail::sync_wait_state<local_sender> state{};
        ASSERT(not state.result);
        ASSERT(not state.error);
        test_std::set_error(test_detail::sync_wait_receiver<local_sender>{&state}, error{17});
        ASSERT(not state.result);
        ASSERT(state.error);
        try {
            std::rethrow_exception(state.error);
        } catch (const error& e) {
            ASSERT(e.value == 17);
        } catch (...) {
            // NOLINTNEXTLINE(cert-dcl03-c,hicpp-static-assert,misc-static-assert)
            ASSERT(nullptr == "unexpected exception type");
        }
    }
    {
        using local_sender = decltype(test_std::just(arg<0>{}, arg<1>{}, arg<2>{}));
        test_detail::sync_wait_state<local_sender> state{};
        ASSERT(not state.result);
        ASSERT(not state.error);
        test_std::set_error(test_detail::sync_wait_receiver<local_sender>{&state}, std::make_exception_ptr(error{17}));
        ASSERT(not state.result);
        ASSERT(state.error);
        try {
            std::rethrow_exception(state.error);
        } catch (const error& e) {
            ASSERT(e.value == 17);
        } catch (...) {
            // NOLINTNEXTLINE(cert-dcl03-c,hicpp-static-assert,misc-static-assert)
            ASSERT(nullptr == "unexpected exception type");
        }
    }
    {
        using local_sender = decltype(test_std::just(arg<0>{}, arg<1>{}, arg<2>{}));
        test_detail::sync_wait_state<local_sender> state{};
        ASSERT(not state.result);
        ASSERT(not state.error);
        test_std::set_stopped(test_detail::sync_wait_receiver<local_sender>{&state});
        ASSERT(not state.result);
        ASSERT(not state.error);
    }
}

auto test_sync_wait() -> void {
    try {
        auto value{test_std::sync_wait(test_std::just(arg<0>{7}, arg<1>{11}))};
        ASSERT(value);
        ASSERT(*value == (std::tuple{arg<0>{7}, arg<1>{11}}));
    } catch (...) {
        // NOLINTBEGIN(cert-dcl03-c,hicpp-static-assert,misc-static-assert)
        ASSERT(nullptr == "no exception expected from sync_wait(just(...)");
        // NOLINTEND(cert-dcl03-c,hicpp-static-assert,misc-static-assert)
    }

    try {
        auto value{test_std::sync_wait(send_error{17})};
        use(value);
        // NOLINTNEXTLINE(cert-dcl03-c,hicpp-static-assert,misc-static-assert)
        ASSERT(nullptr == "this line should never be reached");
    } catch (const error& e) {
        ASSERT(e.value == 17);
    } catch (...) {
        // NOLINTNEXTLINE(cert-dcl03-c,hicpp-static-assert,misc-static-assert)
        ASSERT(nullptr == "no exception expected from sync_wait(just(...)");
    }

    try {
        auto value{test_std::sync_wait(send_stopped())};
        ASSERT(not value);
    } catch (...) {
        // NOLINTBEGIN(cert-dcl03-c,hicpp-static-assert,misc-static-assert)
        ASSERT(nullptr == "no exception expected from sync_wait(just(...)");
        // NOLINTEND(cert-dcl03-c,hicpp-static-assert,misc-static-assert)
    }
}

auto test_provides_scheduler() -> void {
    ASSERT(test_std::sync_wait(test_std::then(test_std::read_env(test_std::get_scheduler), [](auto&&) noexcept {})));
}

auto test_provides_delegation_scheduler() -> void {
    ASSERT(test_std::sync_wait(
        test_std::then(test_std::read_env(test_std::get_delegation_scheduler), [](auto&&) noexcept {})));
}
} // namespace

TEST(exec_sync_wait) {
    static_assert(std::same_as<const test_std::sync_wait_t, decltype(test_std::sync_wait)>);

    test_has_sync_wait<false>(sender{});
    test_has_sync_wait<true>(sender_in{});

    test_sync_wait_env();
    test_sync_wait_result_type();
    test_sync_wait_state();
    test_sync_wait_receiver();
    test_sync_wait();
    test_provides_scheduler();
    test_provides_delegation_scheduler();
}
