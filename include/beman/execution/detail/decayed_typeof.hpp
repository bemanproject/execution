// include/beman/execution/detail/decayed_typeof.hpp                -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_DECAYED_TYPEOF
#define INCLUDED_BEMAN_EXECUTION_DETAIL_DECAYED_TYPEOF

#include <beman/execution/detail/common.hpp>
#include <type_traits>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
/*!
 * \brief Helper type alias to get the type after decaying the argument
 * \headerfile beman/execution/execution.hpp <beman/execution/execution.hpp>
 * \internal
 */
template <auto&& Tag>
using decayed_typeof = ::std::decay_t<decltype(Tag)>;
} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_DECAYED_TYPEOF
