module;
// src/beman/execution/stop_when.cppm                                 -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_HAS_MODULES
#define BEMAN_HAS_MODULES
#endif
#include <beman/execution/detail/stop_when.hpp>

export module beman.execution.detail.stop_when;

namespace beman::execution::detail {
export using beman::execution::detail::stop_when_t;
export using beman::execution::detail::stop_when;
} // namespace beman::execution::detail
