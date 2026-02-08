module;
// src/beman/execution/schedule.cppm                                  -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_HAS_MODULES
#define BEMAN_HAS_MODULES
#endif
#include <beman/execution/detail/schedule.hpp>

export module beman.execution.detail.schedule;

namespace beman::execution {
export using beman::execution::schedule_t;
export using beman::execution::schedule;
} // namespace beman::execution
