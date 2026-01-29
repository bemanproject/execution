// include/beman/execution26/stop_token.hpp                           -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_STOP_TOKEN
#define INCLUDED_BEMAN_EXECUTION26_STOP_TOKEN

#include <beman/execution/stop_token.hpp>

// ----------------------------------------------------------------------------

namespace beman::execution26 {

using ::beman::execution::inplace_stop_callback;
using ::beman::execution::inplace_stop_source;
using ::beman::execution::inplace_stop_token;
using ::beman::execution::never_stop_token;
using ::beman::execution::stop_callback;
using ::beman::execution::stop_callback_for_t;
using ::beman::execution::stop_source;
using ::beman::execution::stop_token;
using ::beman::execution::stoppable_token;
using ::beman::execution::unstoppable_token;

} // namespace beman::execution26

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION26_STOP_TOKEN
