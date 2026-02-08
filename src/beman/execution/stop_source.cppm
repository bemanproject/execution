module;
// src/beman/execution/stop_source.cppm                               -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_HAS_MODULES
#define BEMAN_HAS_MODULES
#endif
#include <beman/execution/detail/stop_source.hpp>

export module beman.execution.detail.stop_source;

namespace beman::execution {
export using beman::execution::stop_callback;
export using beman::execution::stop_source;
export using beman::execution::stop_token;
} // namespace beman::execution
