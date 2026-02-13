module;
// src/beman/execution/then.cppm                                      -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_HAS_MODULES
#define BEMAN_HAS_MODULES
#endif
#include <beman/execution/detail/then.hpp>

export module beman.execution.detail.then;

namespace beman::execution {
export using beman::execution::then_t;
export using beman::execution::then;
export using beman::execution::upon_error_t;
export using beman::execution::upon_error;
export using beman::execution::upon_stopped_t;
export using beman::execution::upon_stopped;
} // namespace beman::execution
