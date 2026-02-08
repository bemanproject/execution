module;
// src/beman/execution/get_completion_scheduler.cppm                  -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_HAS_MODULES
#define BEMAN_HAS_MODULES
#endif
#include <beman/execution/detail/get_completion_scheduler.hpp>

export module beman.execution.detail.get_completion_scheduler;

namespace beman::execution {
export using beman::execution::get_completion_scheduler_t;
export using beman::execution::get_completion_scheduler;
} // namespace beman::execution
