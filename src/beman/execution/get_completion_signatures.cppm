module;
// src/beman/execution/get_completion_signatures.cppm                 -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_HAS_MODULES
#define BEMAN_HAS_MODULES
#endif
#include <beman/execution/detail/get_completion_signatures.hpp>

export module beman.execution.detail.get_completion_signatures;

namespace beman::execution {
export using beman::execution::get_completion_signatures_t;
export using beman::execution::get_completion_signatures;
} // namespace beman::execution
