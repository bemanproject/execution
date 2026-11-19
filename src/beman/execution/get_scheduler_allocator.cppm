module;
// src/beman/execution/get_scheduler_allocator.cppm                   -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/detail/get_scheduler_allocator.hpp>

export module beman.execution.detail.get_scheduler_allocator;

namespace beman::execution::detail {
export using beman::execution::detail::get_scheduler_allocator_t;
} // namespace beman::execution::detail

namespace beman::execution {
export using beman::execution::get_scheduler_allocator_t;
export using beman::execution::get_scheduler_allocator;
} // namespace beman::execution
