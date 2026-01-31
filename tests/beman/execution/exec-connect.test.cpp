// src/beman/execution/tests/exec-connect.test.cpp                  -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/detail/connect.hpp>
#include <beman/execution/detail/receiver.hpp>
#include <beman/execution/detail/operation_state_task.hpp>
#include <beman/execution/detail/suspend_complete.hpp>
#include <beman/execution/detail/connect_awaitable.hpp>
#include <test/execution.hpp>

#include <concepts>
#include <stdexcept>

// ----------------------------------------------------------------------------

namespace {
enum class kind : unsigned char { plain, domain };
template <kind, test_std::receiver Receiver>
struct state {
    using operation_state_concept = test_std::operation_state_t;
    int                           value{};
    std::remove_cvref_t<Receiver> receiver;

    template <typename R>
    state(int val, R&& r) : value(val), receiver(std::forward<R>(r)) {}
    state(const state&)                    = delete;
    state(state&&)                         = delete;
    ~state()                               = default;
    auto operator=(const state&) -> state& = delete;
    auto operator=(state&&) -> state&      = delete;
    auto start() noexcept -> void {}
};

struct sender {
    using sender_concept = test_std::sender_t;
    int value{};

    template <typename Receiver>
    auto connect(Receiver&& receiver) -> state<kind::plain, Receiver> {
        return state<kind::plain, Receiver>(this->value, std::forward<Receiver>(receiver));
    }
};

struct rvalue_sender {
    using sender_concept = test_std::sender_t;
    int value{};

    explicit rvalue_sender(int val) : value(val) {}
    rvalue_sender(const rvalue_sender&)                    = delete;
    rvalue_sender(rvalue_sender&&)                         = default;
    auto operator=(const rvalue_sender&) -> rvalue_sender& = delete;
    auto operator=(rvalue_sender&&) -> rvalue_sender&      = default;
    ~rvalue_sender()                                       = default;

    template <typename Receiver>
    auto connect(Receiver&& receiver) && -> state<kind::plain, Receiver> {
        return state<kind::plain, Receiver>(this->value, std::forward<Receiver>(receiver));
    }
};

struct env {
    int value{};
};

struct receiver {
    using receiver_concept = test_std::receiver_t;
    int   value{};
    bool* set_stopped_called{};

    explicit receiver(int val, bool* called = {}) : value(val), set_stopped_called(called) {}
    receiver(receiver&&)                           = default;
    receiver(const receiver&)                      = delete;
    ~receiver()                                    = default;
    auto operator=(receiver&&) -> receiver&        = default;
    auto operator=(const receiver&) -> receiver&   = delete;
    auto operator==(const receiver&) const -> bool = default;

    auto get_env() const noexcept -> env { return {this->value + 2}; }
    auto set_stopped() && noexcept -> void { this->set_stopped_called && (*this->set_stopped_called = true); }
};

struct domain_sender {
    using sender_concept = test_std::sender_t;
    int value{};

    template <typename Receiver>
    auto connect(Receiver&& receiver) && -> state<kind::domain, Receiver> {
        return state<kind::domain, Receiver>(this->value, std::forward<Receiver>(receiver));
    }
};

struct domain {
    auto transform_sender(auto&& sender, auto&&...) const noexcept -> domain_sender {
        return domain_sender{sender.value};
    }
};

struct domain_env {
    auto query(const test_std::get_domain_t&) const noexcept -> domain { return {}; }
};

struct domain_receiver {
    using receiver_concept = test_std::receiver_t;
    int value{};

    explicit domain_receiver(int val) : value(val) {}
    domain_receiver(domain_receiver&&)                         = default;
    domain_receiver(const domain_receiver&)                    = delete;
    ~domain_receiver()                                         = default;
    auto operator=(domain_receiver&&) -> domain_receiver&      = default;
    auto operator=(const domain_receiver&) -> domain_receiver& = delete;

    auto operator==(const domain_receiver&) const -> bool = default;

    auto get_env() const noexcept -> domain_env { return {}; }
};

struct awaiter {
    auto await_ready() -> bool { return {}; }
    auto await_suspend(auto) -> bool { return {}; }
    auto await_resume() -> int { return {}; }
};

struct transformed {
    auto as_awaitable(auto&&) -> awaiter { return {}; }
};

auto test_connect_awaitable_promise() -> void {
    using connect_awaitable_promise = test_detail::operation_state_task<receiver>::promise_type;

    bool     set_stopped_called{false};
    receiver r{17, &set_stopped_called};
    static_assert(noexcept(connect_awaitable_promise(0, r)));
    test_detail::connect_awaitable_promise<receiver> promise(0, r);
    static_assert(std::same_as<awaiter, ::std::remove_cvref_t<decltype(promise.await_transform(awaiter{}))>>);
    static_assert(std::same_as<awaiter, ::std::remove_cvref_t<decltype(promise.await_transform(transformed{}))>>);

    static_assert(noexcept(promise.initial_suspend()));
    static_assert(std::same_as<std::suspend_always, decltype(promise.initial_suspend())>);
    static_assert(noexcept(promise.final_suspend()));
    static_assert(std::same_as<std::suspend_always, decltype(promise.final_suspend())>);
    static_assert(noexcept(promise.unhandled_exception()));
    static_assert(std::same_as<void, decltype(promise.unhandled_exception())>);
    static_assert(noexcept(promise.return_void()));
    static_assert(std::same_as<void, decltype(promise.return_void())>);

    static_assert(noexcept(std::as_const(promise).get_env()));
    static_assert(std::same_as<env, decltype(promise.get_env())>);
    ASSERT(19 == promise.get_env().value);

    static_assert(noexcept(promise.unhandled_stopped()));
    static_assert(std::same_as<std::coroutine_handle<>, decltype(promise.unhandled_stopped())>);
    ASSERT(set_stopped_called == false);
    auto handle{promise.unhandled_stopped()};
    ASSERT(set_stopped_called == true && handle == std::noop_coroutine());

    static_assert(noexcept(promise.get_return_object()));
    static_assert(std::same_as<test_detail::operation_state_task<receiver>, decltype(promise.get_return_object())>);

    test::use(promise);
}

auto test_operation_state_task() -> void {
    using state_t = test_detail::operation_state_task<receiver>;
    static_assert(std::same_as<::beman::execution::operation_state_t, state_t::operation_state_concept>);
    static_assert(std::same_as<test_detail::connect_awaitable_promise<receiver>, state_t::promise_type>);
    static_assert(noexcept(state_t(std::coroutine_handle<>{})));
    state_t state(::std::coroutine_handle<>{});
    static_assert(noexcept(state.start()));
}

auto test_suspend_complete() -> void {
    static_assert(noexcept(test_detail::suspend_complete([](int) {}, 17)));
    int  iv{};
    bool bv{};
    int  ip{17};
    bool bp{true};
    auto awaiter{test_detail::suspend_complete(
        [&](int i, bool b) {
            iv = i;
            bv = b;
        },
        ip,
        bp)};

    static_assert(noexcept(awaiter.await_ready()));
    static_assert(false == awaiter.await_ready());
    static_assert(noexcept(awaiter.await_suspend(std::noop_coroutine())));
    static_assert(std::same_as<void, decltype(awaiter.await_suspend(std::noop_coroutine()))>);
    ASSERT(iv == 0);
    ASSERT(bv == false);
    awaiter.await_suspend(std::noop_coroutine());
    ASSERT(iv == 17);
    ASSERT(bv == true);

    static_assert(noexcept(awaiter.await_resume()));
    static_assert(std::same_as<void, decltype(awaiter.await_resume())>);
}

auto test_connect_awaitable() -> void {
    struct local_awaiter {
        ::std::coroutine_handle<>& handle;
        int&                       result;

        auto await_ready() -> bool { return {}; }
        auto await_suspend(::std::coroutine_handle<> h) -> void { this->handle = h; }
        auto await_resume() {
            switch (result) {
            default:
                return result;
            case 0:
                throw ::std::runtime_error("exception");
            }
        }
    };

    struct void_awaiter {
        ::std::coroutine_handle<>& handle;

        auto await_ready() -> bool { return {}; }
        auto await_suspend(::std::coroutine_handle<> h) -> void { this->handle = h; }
        auto await_resume() -> void {}
    };

    struct local_receiver {
        using receiver_concept = test_std::receiver_t;

        int&  iv;
        bool& bv;
        auto  set_value() && noexcept -> void { this->bv = true; }
        auto  set_value(int value) && noexcept -> void {
            this->iv = value;
            this->bv = true;
        }
        auto set_error(const ::std::exception_ptr& error) && noexcept -> void {
            try {
                std::rethrow_exception(error);
            } catch (const std::runtime_error&) {
                this->bv = true;
            } catch (...) {
                this->bv = false;
            }
        }
        auto set_stopped() && noexcept -> void {}
    };

    {
        ::std::coroutine_handle<> handle{};
        int                       result{};
        int                       iv{};
        bool                      bv{};

        auto op1{test_detail::connect_awaitable(local_awaiter{handle, result}, local_receiver{iv, bv})};
        ASSERT(handle == std::coroutine_handle<>());
        op1.start();
        ASSERT(handle != std::coroutine_handle<>());
        ASSERT(iv == 0 && bv == false);
        result = 42;
        test::use(result);
        handle.resume();
        ASSERT(iv == 42 && bv == true);
    }
    {
        ::std::coroutine_handle<> handle{};
        int                       result{};
        int                       iv{};
        bool                      bv{};

        auto op1{test_detail::connect_awaitable(local_awaiter{handle, result}, local_receiver{iv, bv})};
        ASSERT(handle == std::coroutine_handle<>());
        op1.start();
        ASSERT(handle != std::coroutine_handle<>());
        ASSERT(iv == 0 && bv == false);
        result = 0;
        test::use(result);
        handle.resume();
        ASSERT(iv == 0 && bv == true);
    }

    {
        ::std::coroutine_handle<> handle{};
        int                       iv{};
        bool                      bv{};

        auto op1{test_detail::connect_awaitable(void_awaiter{handle}, local_receiver{iv, bv})};
        ASSERT(handle == std::coroutine_handle<>());
        op1.start();
        ASSERT(handle != std::coroutine_handle<>());
        ASSERT(iv == 0 && bv == false);
        handle.resume();
        ASSERT(iv == 0 && bv == true);
    }
}

auto test_connect_with_awaiter() -> void {
    struct local_awaiter {
        ::std::coroutine_handle<>& handle;
        auto                       await_ready() -> bool { return {}; }
        auto                       await_suspend(std::coroutine_handle<> h) -> void { this->handle = h; }
        auto                       await_resume() -> int { return 17; }
    };
    struct local_receiver {
        using receiver_concept = test_std::receiver_t;
        bool& result;
        auto  set_value(int i) && noexcept -> void { this->result = i == 17; }
        auto  set_error(const std::exception_ptr&) && noexcept -> void {}
        auto  set_stopped() && noexcept -> void {}
    };

    std::coroutine_handle<> handle{};
    bool                    result{};
    auto                    op{test_std::connect(local_awaiter{handle}, local_receiver{result})};
    ASSERT(handle == std::coroutine_handle{});
    test_std::start(op);
    ASSERT(handle != std::coroutine_handle{});
    ASSERT(result == false);
    handle.resume();
    ASSERT(result == true);
}
} // namespace

TEST(exec_connect) {
    static_assert(std::same_as<const test_std::connect_t, decltype(test_std::connect)>);

    static_assert(test_std::operation_state<state<kind::plain, receiver>>);
    static_assert(test_std::sender<sender>);
    static_assert(test_std::sender<rvalue_sender>);
    static_assert(test_std::receiver<receiver>);

    {
        static_assert(
            std::same_as<state<kind::plain, receiver>, decltype(test_std::connect(sender{43}, receiver(17)))>);
        auto op{test_std::connect(sender{43}, receiver(17))};
        ASSERT(op.value == 43);
        ASSERT(op.receiver == receiver(17));
    }

    {
        static_assert(
            std::same_as<state<kind::plain, receiver>, decltype(test_std::connect(rvalue_sender(42), receiver(17)))>);
        auto op{test_std::connect(rvalue_sender(42), receiver(17))};
        ASSERT(op.value == 42);
        ASSERT(op.receiver == receiver(17));
    }

    {
        static_assert(std::same_as<domain, decltype(test_std::get_domain(domain_env()))>);
        static_assert(std::same_as<domain_env, decltype(test_std::get_env(domain_receiver(17)))>);
        static_assert(
            std::same_as<domain_sender, decltype(domain().transform_sender(sender{42}, domain_receiver(17)))>);
        static_assert(
            std::same_as<domain_sender, decltype(test_std::transform_sender(domain(), sender{42}, domain_env()))>);

        static_assert(std::same_as<state<kind::domain, domain_receiver>,
                                   decltype(test_std::connect(sender{42}, domain_receiver(17)))>);
        auto op{test_std::connect(sender{42}, domain_receiver(17))};
        ASSERT(op.value == 42);
        ASSERT(op.receiver == domain_receiver(17));
    }

    test_connect_awaitable_promise();
    test_operation_state_task();
    test_suspend_complete();
    test_connect_awaitable();
    test_connect_with_awaiter();
}
