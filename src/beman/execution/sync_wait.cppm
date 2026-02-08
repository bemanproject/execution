module;
// src/beman/execution/sync_wait.cppm                                 -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_HAS_MODULES
#define BEMAN_HAS_MODULES
#endif
#include <beman/execution/detail/sync_wait.hpp>

export module beman.execution.detail.sync_wait;

namespace beman::execution::detail {
export using beman::execution::detail::sync_wait_env;
export using beman::execution::detail::sync_wait_receiver;
export using beman::execution::detail::sync_wait_result_type;
export using beman::execution::detail::sync_wait_state;
} // namespace beman::execution::detail
namespace beman::execution {
export using beman::execution::sync_wait_t;
export using beman::execution::sync_wait;
} // namespace beman::execution
