// include/beman/execution/detail/completion_signaturess_of_t.hpp   -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_COMPLETION_SIGNATURES_OF
#define INCLUDED_BEMAN_EXECUTION_DETAIL_COMPLETION_SIGNATURES_OF

#include <beman/execution/detail/config.hpp>
#include <beman/execution/detail/call_result_t.hpp>
#include <beman/execution/detail/env.hpp>
#include <beman/execution/detail/get_completion_signatures.hpp>
#include <beman/execution/detail/sender_in.hpp>

// ----------------------------------------------------------------------------

namespace beman::execution {
/*!
 * \brief Alias to access the completion signatures of a sender
 * \headerfile beman/execution/execution.hpp <beman/execution/execution.hpp>
 */
BEMAN_EXECUTION_EXPORT template <typename Sender, typename Env = ::beman::execution::env<>>
    requires ::beman::execution::sender_in<Sender, Env>
using completion_signatures_of_t =
    ::beman::execution::detail::call_result_t<::beman::execution::get_completion_signatures_t, Sender, Env>;
} // namespace beman::execution

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_COMPLETION_SIGNATURES_OF
