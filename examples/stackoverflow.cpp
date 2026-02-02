// examples/allocator.cpp                                             -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <coroutine>
#include <iostream>
#include <memory>
#include <type_traits>
#include <utility>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution/execution.hpp>
#endif

namespace ex = beman::execution;

struct task {
    using sender_concept        = ex::sender_t;
    using completion_signatures = ex::completion_signatures<ex::set_value_t()>;

    struct base {
        virtual void complete_value() noexcept   = 0;
        virtual void complete_stopped() noexcept = 0;
    };

    struct promise_type {
        struct final_awaiter {
            base* data;
            bool  await_ready() noexcept { return false; }
            auto  await_suspend(auto) noexcept { this->data->complete_value(); };
            void  await_resume() noexcept {}
        };
        std::suspend_always     initial_suspend() const noexcept { return {}; }
        final_awaiter           final_suspend() const noexcept { return {this->data}; }
        void                    unhandled_exception() const noexcept {}
        std::coroutine_handle<> unhandled_stopped() {
            this->data->complete_stopped();
            return std::noop_coroutine();
        }
        auto return_void() {}
        auto get_return_object() { return task{std::coroutine_handle<promise_type>::from_promise(*this)}; }
        template <::beman::execution::sender Sender>
        auto await_transform(Sender&& sender) noexcept {
            return ::beman::execution::as_awaitable(::std::forward<Sender>(sender), *this);
        }

        base* data{};
    };

    template <ex::receiver R>
    struct state : base {

        using operation_state_concept = ex::operation_state_t;
        std::remove_cvref_t<R>              r;
        std::coroutine_handle<promise_type> handle;

        state(auto&& r, auto&& h) : r(std::forward<R>(r)), handle(std::move(h)) {}
        void start() & noexcept {
            this->handle.promise().data = this;
            this->handle.resume();
        }
        void complete_value() noexcept override {
            this->handle.destroy();
            ex::set_value(std::move(this->r));
        }
        void complete_stopped() noexcept override {
            this->handle.destroy();
            ex::set_stopped(std::move(this->r));
        }
    };

    std::coroutine_handle<promise_type> handle;

    template <ex::receiver R>
    auto connect(R&& r) && {
        return state<R>(std::forward<R>(r), std::move(this->handle));
    }
};

int main(int ac, char*[]) {
    std::cout << std::unitbuf;
#ifndef _MSC_VER
    using on_exit = std::unique_ptr<const char, decltype([](auto msg) { std::cout << msg << "\n"; })>;
    static_assert(ex::sender<task>);
    ex::sync_wait([](int n) -> task {
        on_exit msg("coro run to the end");
        if constexpr (true)
            for (int i{}; i < n; ++i) {
                std::cout << "await just=" << (co_await ex::just(i)) << "\n";
            }
        if constexpr (false)
            for (int i{}; i < n; ++i) {
                try {
                    co_await ex::just_error(i);
                } catch (int x) {
                    std::cout << "await error=" << x << "\n";
                }
            }
        co_await ex::just_stopped();
    }(ac < 2 ? 3 : 30000));
#endif
}
