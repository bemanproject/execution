module;
// src/beman/execution/task_scheduler.cppm                          -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/detail/task_scheduler.hpp>
#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <memory>
#include <type_traits>
#endif

export module beman.execution.detail.task_scheduler;

namespace beman::execution {
export using beman::execution::task_scheduler;
} // namespace beman::execution

template <typename Alloc>
struct std::uses_allocator<::beman::execution::task_scheduler, Alloc> : ::std::true_type {};
