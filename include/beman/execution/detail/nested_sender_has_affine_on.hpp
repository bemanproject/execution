// include/beman/execution/detail/nested_sender_has_affine_on.hpp     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_NESTED_SENDER_HAS_AFFINE_ON
#define INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_NESTED_SENDER_HAS_AFFINE_ON

#include <beman/execution/detail/sender_has_affine_on.hpp>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
template <typename Sender, typename Env>
concept nested_sender_has_affine_on = requires(Sender&& sndr, const Env& env) {
    { sndr.template get<2>() } -> ::beman::execution::detail::sender_has_affine_on<Env>;
};
} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif
