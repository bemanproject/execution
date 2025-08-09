// include/beman/execution/detail/sender_awaitable.hpp              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_SENDER_AWAITABLE
#define INCLUDED_BEMAN_EXECUTION_DETAIL_SENDER_AWAITABLE

#include <beman/execution/detail/as_except_ptr.hpp>
#include <beman/execution/detail/connect_result_t.hpp>
#include <beman/execution/detail/connect.hpp>
#include <beman/execution/detail/env_promise.hpp>
#include <beman/execution/detail/fwd_env.hpp>
#include <beman/execution/detail/get_env.hpp>
#include <beman/execution/detail/is_awaitable.hpp>
#include <beman/execution/detail/single_sender_value_type.hpp>
#include <beman/execution/detail/single_sender.hpp>
#include <beman/execution/detail/start.hpp>
#include <beman/execution/detail/unspecified_promise.hpp>

#include <concepts>
#include <coroutine>
#include <exception>
#include <type_traits>
#include <utility>
#include <variant>

namespace beman::execution::detail {
template <class Sndr, class Promise>
class sender_awaitable {
    struct unit {};
    using value_type =
        ::beman::execution::detail::single_sender_value_type<Sndr, ::beman::execution::env_of_t<Promise>>;
    using result_type  = ::std::conditional_t<::std::is_void_v<value_type>, unit, value_type>;
    using variant_type = ::std::variant<::std::monostate, result_type, ::std::exception_ptr>;
    struct awaitable_receiver {
        using receiver_concept = ::beman::execution::receiver_t;

        template <class... Args>
            requires ::std::constructible_from<result_type, Args...>
        void set_value(Args&&... args) && noexcept {
            try {
                result_ptr_->template emplace<1>(::std::forward<Args>(args)...);
            } catch (...) {
                result_ptr_->template emplace<2>(::std::current_exception());
            }
            continuation_.resume();
        }

        template <class Error>
        void set_error(Error&& error) && noexcept {
            result_ptr_->template emplace<2>(::beman::execution::detail::as_except_ptr(::std::forward<Error>(error)));
            continuation_.resume();
        }

        void set_stopped() && noexcept {
            static_cast<::std::coroutine_handle<>>(continuation_.promise().unhandled_stopped()).resume();
        }

        auto get_env() const noexcept {
            return ::beman::execution::detail::fwd_env{::beman::execution::get_env(continuation_.promise())};
        }

        variant_type*                    result_ptr_;
        ::std::coroutine_handle<Promise> continuation_;
    };
    using op_state_type = ::beman::execution::connect_result_t<Sndr, awaitable_receiver>;

    variant_type  result{};
    op_state_type state;

  public:
    sender_awaitable(Sndr&& sndr, Promise& p)
        : state{::beman::execution::connect(
              ::std::forward<Sndr>(sndr),
              awaitable_receiver{::std::addressof(result), ::std::coroutine_handle<Promise>::from_promise(p)})} {}

    static constexpr bool await_ready() noexcept { return false; }
    void       await_suspend(::std::coroutine_handle<Promise>) noexcept { ::beman::execution::start(state); }
    value_type await_resume() {
        if (::std::holds_alternative<::std::exception_ptr>(result)) {
            ::std::rethrow_exception(::std::get<::std::exception_ptr>(result));
        }
        if constexpr (::std::is_void_v<value_type>) {
            return;
        } else {
            return ::std::get<value_type>(std::move(result));
        }
    }
};
} // namespace beman::execution::detail

#endif
