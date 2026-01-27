// include/beman/execution/detail/receiver_of.hpp                   -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_RECEIVER_OF
#define INCLUDED_BEMAN_EXECUTION_DETAIL_RECEIVER_OF

#include <beman/execution/detail/has_completions.hpp>
#include <beman/execution/detail/receiver.hpp>

// ----------------------------------------------------------------------------

namespace beman::execution {
template <typename Receiver, typename Completions>
concept receiver_of =
    beman::execution::receiver<Receiver> && beman::execution::detail::has_completions<Receiver, Completions>;

}

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_RECEIVER_OF
