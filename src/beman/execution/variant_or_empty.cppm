module;
// src/beman/execution/variant_or_empty.cppm                          -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_HAS_MODULES
#define BEMAN_HAS_MODULES
#endif
#include <beman/execution/detail/variant_or_empty.hpp>

export module beman.execution.detail.variant_or_empty;

namespace beman::execution::detail {
export using beman::execution::detail::empty_variant;
export using beman::execution::detail::variant_or_empty;
} // namespace beman::execution::detail
