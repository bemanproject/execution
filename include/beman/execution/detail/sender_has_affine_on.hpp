// include/beman/execution/detail/sender_has_affine_on.hpp            -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_SENDER_HAS_AFFINE_ON
#define INCLUDED_BEMAN_EXECUTION_DETAIL_SENDER_HAS_AFFINE_ON

#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <type_traits>
#include <utility>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail.sender
#else
#include <beman/execution/detail/sender.hpp>
#endif

    // ----------------------------------------------------------------------------

    namespace beman::execution::detail { template <typename Sender, typename Env> concept sender_has_affine_on = beman::execution::sender <::std::remove_cvref_t<Sender>> && requires(Sender && sndr, const Env& env) { sndr.template get <0>();
{ sndr.template get<0>().affine_on(std::forward<Sender>(sndr), env) } -> ::beman::execution::sender;
};
} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_SENDER_HAS_AFFINE_ON
