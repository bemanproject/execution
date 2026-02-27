// include/beman/execution/detail/as_awaitable.hpp                  -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_AS_AWAITABLE
#define INCLUDED_BEMAN_EXECUTION_DETAIL_AS_AWAITABLE

#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <coroutine>
#include <functional>
#include <type_traits>
#include <utility>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail.awaitable_sender;
import beman.execution.detail.get_await_completion_adaptor;
import beman.execution.detail.get_env;
import beman.execution.detail.query_with_default;
import beman.execution.detail.is_awaitable;
import beman.execution.detail.sender;
import beman.execution.detail.sender_awaitable;
import beman.execution.detail.unspecified_promise;
#else
#include <beman/execution/detail/awaitable_sender.hpp>
#include <beman/execution/detail/get_await_completion_adaptor.hpp>
#include <beman/execution/detail/get_env.hpp>
#include <beman/execution/detail/query_with_default.hpp>
#include <beman/execution/detail/is_awaitable.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/sender_awaitable.hpp>
#include <beman/execution/detail/unspecified_promise.hpp>
#endif

// ----------------------------------------------------------------------------

namespace beman::execution {
/*!
 * \brief Turn an entity, e.g., a sender, into an awaitable.
 * \headerfile beman/execution/execution.hpp <beman/execution/execution.hpp>
 */
struct as_awaitable_t {
    template <typename Expr, typename Promise>
    auto operator()(Expr&& expr, Promise& promise) const {
        if constexpr (requires { ::std::forward<Expr>(expr).as_awaitable(promise); }) {
            static_assert(
                ::beman::execution::detail::is_awaitable<decltype(::std::forward<Expr>(expr).as_awaitable(promise)),
                                                         Promise>,
                "as_awaitable must return an awaitable");
            return ::std::forward<Expr>(expr).as_awaitable(promise);
        } else if constexpr (::beman::execution::sender<Expr> &&
                             !::beman::execution::detail::
                                 is_awaitable<Expr, ::beman::execution::detail::unspecified_promise>) {
            auto adaptor =
                ::beman::execution::detail::query_with_default(::beman::execution::get_await_completion_adaptor,
                                                               ::beman::execution::get_env(expr),
                                                               ::std::identity{});
            using sender_t = ::std::invoke_result_t<decltype(adaptor), Expr>;
            if constexpr (::beman::execution::detail::awaitable_sender<sender_t, Promise>) {
                return ::beman::execution::detail::sender_awaitable<sender_t, Promise>{
                    adaptor(::std::forward<Expr>(expr)), promise};
            } else if constexpr (::beman::execution::detail::awaitable_sender<Expr, Promise>) {
                return ::beman::execution::detail::sender_awaitable<Expr, Promise>{::std::forward<Expr>(expr),
                                                                                   promise};
            } else {
                return ::std::forward<Expr>(expr);
            }
        } else {
            return ::std::forward<Expr>(expr);
        }
    }
};
inline constexpr ::beman::execution::as_awaitable_t as_awaitable{};
} // namespace beman::execution

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_AS_AWAITABLE
