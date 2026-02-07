module;
// src/beman/execution/spawn_future.cppm                              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/detail/spawn_future.hpp>

export module beman.execution.detail.spawn_future;

namespace beman::execution::detail {
export using beman::execution::detail::spawn_future_receiver;
export using beman::execution::detail::spawn_future_state_base;
} // namespace beman::execution::detail
namespace beman::execution {
export using beman::execution::spawn_future_t;
export using beman::execution::spawn_future;
} // namespace beman::execution
