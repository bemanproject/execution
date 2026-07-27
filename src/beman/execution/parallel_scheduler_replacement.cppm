module;
// src/beman/execution/parallel_scheduler_replacement.cppm                     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/detail/parallel_scheduler_replacement.hpp>

export module beman.execution.detail.parallel_scheduler_replacement;

namespace beman::execution::parallel_scheduler_replacement {
export using beman::execution::parallel_scheduler_replacement::receiver_proxy;
export using beman::execution::parallel_scheduler_replacement::bulk_item_receiver_proxy;
export using beman::execution::parallel_scheduler_replacement::parallel_scheduler_backend;
export using beman::execution::parallel_scheduler_replacement::query_parallel_scheduler_backend;
} // namespace beman::execution::parallel_scheduler_replacement
