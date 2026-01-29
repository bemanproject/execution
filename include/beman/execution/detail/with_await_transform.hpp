// include/beman/execution/detail/with_await_transform.hpp          -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_WITH_AWAIT_TRANSFORM
#define INCLUDED_BEMAN_EXECUTION_DETAIL_WITH_AWAIT_TRANSFORM

#include <beman/execution/detail/common.hpp>
#include <beman/execution/detail/has_as_awaitable.hpp>
#include <utility>
#include <type_traits>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
// NOLINTBEGIN(bugprone-crtp-constructor-accessibility)
template <typename Derived>
struct with_await_transform {
    template <typename T>
    auto await_transform(T&& obj) noexcept -> T&& {
        return ::std::forward<T>(obj);
    }

    template <::beman::execution::detail::has_as_awaitable<Derived> T>
    auto await_transform(T&& obj) noexcept(noexcept(::std::forward<T>(obj).as_awaitable(::std::declval<Derived&>())))
        -> decltype(auto) {
        return ::std::forward<T>(obj).as_awaitable(static_cast<Derived&>(*this));
    }
};
// NOLINTEND(bugprone-crtp-constructor-accessibility)
} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_WITH_AWAIT_TRANSFORM
