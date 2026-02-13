module;
// src/beman/execution/write_env.cppm                                 -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_HAS_MODULES
#define BEMAN_HAS_MODULES
#endif
#include <beman/execution/detail/write_env.hpp>

export module beman.execution.detail.write_env;

namespace beman::execution {
export using beman::execution::write_env_t;
export using beman::execution::write_env;
} // namespace beman::execution
