// include/beman/execution/detail/as_awaitable.hpp                  -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_AS_AWAITABLE
#define INCLUDED_BEMAN_EXECUTION_DETAIL_AS_AWAITABLE

#include <beman/execution/detail/config.hpp>
#include <beman/execution/detail/awaitable_sender.hpp>
#include <beman/execution/detail/is_awaitable.hpp>
#include <beman/execution/detail/sender_awaitable.hpp>
#include <beman/execution/detail/unspecified_promise.hpp>

#include <coroutine>
#include <type_traits>
#include <utility>

// ----------------------------------------------------------------------------

namespace beman::execution {
/*!
 * \brief Turn an entity, e.g., a sender, into an awaitable.
 * \headerfile beman/execution/execution.hpp <beman/execution/execution.hpp>
 */
BEMAN_EXECUTION_EXPORT struct as_awaitable_t {
    template <typename Expr, typename Promise>
    auto operator()(Expr&& expr, Promise& promise) const {
        if constexpr (requires { ::std::forward<Expr>(expr).as_awaitable(promise); }) {
            static_assert(
                ::beman::execution::detail::is_awaitable<decltype(::std::forward<Expr>(expr).as_awaitable(promise)),
                                                         Promise>,
                "as_awaitable must return an awaitable");
            return ::std::forward<Expr>(expr).as_awaitable(promise);
        } else if constexpr (::beman::execution::detail::
                                 is_awaitable<Expr, ::beman::execution::detail::unspecified_promise> ||
                             !::beman::execution::detail::awaitable_sender<Expr, Promise>) {
            return ::std::forward<Expr>(expr);
        } else {
            return ::beman::execution::detail::sender_awaitable<Expr, Promise>{::std::forward<Expr>(expr), promise};
        }
    }
};
BEMAN_EXECUTION_EXPORT inline constexpr ::beman::execution::as_awaitable_t as_awaitable{};
} // namespace beman::execution

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_AS_AWAITABLE
