// include/beman/execution/detail/connect_all_result.hpp            -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_CONNECT_ALL_RESULT
#define INCLUDED_BEMAN_EXECUTION_DETAIL_CONNECT_ALL_RESULT

#include <beman/execution/detail/common.hpp>
#include <beman/execution/detail/basic_state.hpp>
#include <beman/execution/detail/connect_all.hpp>
#include <beman/execution/detail/call_result_t.hpp>
#include <beman/execution/detail/indices_for.hpp>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
/*!
 * \brief Helper type used to determine the state type when connecting all senders in a basic_sender
 * \headerfile beman/execution/execution.hpp <beman/execution/execution.hpp>
 * \internal
 */
template <typename Sender, typename Receiver>
using connect_all_result =
    ::beman::execution::detail::call_result_t<decltype(::beman::execution::detail::connect_all),
                                              ::beman::execution::detail::basic_state<Sender, Receiver>*,
                                              Sender,
                                              ::beman::execution::detail::indices_for<Sender>>;
} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_CONNECT_ALL_RESULT
