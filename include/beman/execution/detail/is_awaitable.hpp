// include/beman/execution/detail/is_awaitable.hpp                  -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_IS_AWAITABLE
#define INCLUDED_BEMAN_EXECUTION_DETAIL_IS_AWAITABLE

#include <beman/execution/detail/common.hpp>
#include <beman/execution/detail/get_awaiter.hpp>
#include <beman/execution/detail/is_awaiter.hpp>
#include <type_traits>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
template <typename T, typename Promise>
concept is_awaitable = requires(Promise& promise) {
    {
        ::beman::execution::detail::get_awaiter(::std::declval<T>(), promise)
    } -> ::beman::execution::detail::is_awaiter<Promise>;
};
} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_IS_AWAITABLE
