// include/beman/execution/detail/queryable.hpp                     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_QUERYABLE
#define INCLUDED_BEMAN_EXECUTION_DETAIL_QUERYABLE

#include <beman/execution/detail/common.hpp>
#include <concepts>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
template <typename T>
concept queryable = ::std::destructible<T>;
}

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_QUERYABLE
