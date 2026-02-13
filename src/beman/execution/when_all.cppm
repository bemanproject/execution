module;
// src/beman/execution/when_all.cppm                                  -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_HAS_MODULES
#define BEMAN_HAS_MODULES
#endif
#include <beman/execution/detail/when_all.hpp>

export module beman.execution.detail.when_all;

namespace beman::execution {
export using beman::execution::when_all_t;
export using beman::execution::when_all;
} // namespace beman::execution
