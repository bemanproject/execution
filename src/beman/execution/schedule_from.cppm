module;
// src/beman/execution/schedule_from.cppm                             -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_HAS_MODULES
#define BEMAN_HAS_MODULES
#endif
#include <beman/execution/detail/schedule_from.hpp>

export module beman.execution.detail.schedule_from;

namespace beman::execution {
export using beman::execution::schedule_from_t;
export using beman::execution::schedule_from;
} // namespace beman::execution

namespace beman::execution::detail {
export BEMAN_SPECIALIZE_EXPORT struct impls_for<::beman::execution::schedule_from_t>;
} // namespace beman::execution::detail
