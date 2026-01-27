// include/beman/execution/detail/call_result_t.hpp                 -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_CALL_RESULT
#define INCLUDED_BEMAN_EXECUTION_DETAIL_CALL_RESULT

#include <utility>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
template <typename Fun, typename... Args>
/*!
 * \brief Type alias used determine the result of function [object] call.
 * \headerfile beman/execution/execution.hpp <beman/execution/execution.hpp>
 * \internal
 */
using call_result_t = decltype(::std::declval<Fun>()(std::declval<Args>()...));
}

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_CALL_RESULT
