module;
// src/beman/execution/into_variant.cppm                              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_HAS_MODULES
#define BEMAN_HAS_MODULES
#endif
#include <beman/execution/detail/into_variant.hpp>

export module beman.execution.detail.into_variant;

namespace beman::execution {
export using beman::execution::into_variant;
export using beman::execution::into_variant_t;
} // namespace beman::execution

namespace beman::execution::detail {
export BEMAN_SPECIALIZE_EXPORT struct impls_for<::beman::execution::detail::into_variant_t>;
} // namespace beman::execution::detail
