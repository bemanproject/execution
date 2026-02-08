module;
// src/beman/execution/forward_like.cppm                              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_HAS_MODULES
#define BEMAN_HAS_MODULES
#endif
#include <beman/execution/detail/forward_like.hpp>

export module beman.execution.detail.forward_like;

namespace beman::execution::detail {
export using beman::execution::detail::own_forward_like;
export using beman::execution::detail::forward_like;
} // namespace beman::execution::detail
