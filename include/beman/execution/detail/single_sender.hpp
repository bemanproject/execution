// include/beman/execution/detail/single_sender.hpp                 -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_SINGLE_SENDER
#define INCLUDED_BEMAN_EXECUTION_DETAIL_SINGLE_SENDER

#include <beman/execution/detail/common.hpp>
#include <beman/execution/detail/sender_in.hpp>
#include <beman/execution/detail/single_sender_value_type.hpp>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
template <typename Sender, typename Env>
concept single_sender = ::beman::execution::sender_in<Sender, Env> &&
                        requires { typename ::beman::execution::detail::single_sender_value_type<Sender, Env>; };
} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_SINGLE_SENDER
