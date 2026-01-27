// include/beman/execution/detail/connect_result_t.hpp              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_CONNECT_RESULT
#define INCLUDED_BEMAN_EXECUTION_DETAIL_CONNECT_RESULT

#include <beman/execution/detail/connect.hpp>
#include <type_traits>

// ----------------------------------------------------------------------------

namespace beman::execution {
/*!
 * \brief Type alias to determine the operation state type returned from `connect()`.
 * \headerfile beman/execution/execution.hpp <beman/execution/execution.hpp>
 */
template <typename Sender, typename Receiver>
using connect_result_t = decltype(::beman::execution::connect(::std::declval<Sender>(), ::std::declval<Receiver>()));
} // namespace beman::execution

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_CONNECT_RESULT
