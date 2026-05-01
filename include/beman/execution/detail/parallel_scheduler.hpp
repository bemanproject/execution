// include/beman/execution/detail/parallel_scheduler.hpp            -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_PARALLEL_SCHEDULER
#define INCLUDED_BEMAN_EXECUTION_DETAIL_PARALLEL_SCHEDULER

#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <cstddef>
#include <concepts>
#include <exception>
#include <memory>
#include <optional>
#include <span>
#include <type_traits>
#include <utility>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail.completion_signatures;
import beman.execution.detail.get_completion_scheduler;
import beman.execution.detail.get_forward_progress_guarantee;
import beman.execution.detail.operation_state;
import beman.execution.detail.receiver;
import beman.execution.detail.scheduler;
import beman.execution.detail.scheduler_t;
import beman.execution.detail.sender;
import beman.execution.detail.set_error;
import beman.execution.detail.set_stopped;
import beman.execution.detail.set_value;
#else
#include <beman/execution/detail/completion_signatures.hpp>
#include <beman/execution/detail/get_completion_scheduler.hpp>
#include <beman/execution/detail/get_forward_progress_guarantee.hpp>
#include <beman/execution/detail/operation_state.hpp>
#include <beman/execution/detail/receiver.hpp>
#include <beman/execution/detail/scheduler.hpp>
#include <beman/execution/detail/scheduler_t.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/set_error.hpp>
#include <beman/execution/detail/set_stopped.hpp>
#include <beman/execution/detail/set_value.hpp>
#endif

// ----------------------------------------------------------------------------

namespace beman::execution {

class parallel_scheduler {
    // TODO(P2079R10): add scheduler state and operations.
};

// TODO(P2079R10): implement using system_context_replaceability::query_parallel_scheduler_backend().
auto get_parallel_scheduler() -> parallel_scheduler;

} // namespace beman::execution

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_PARALLEL_SCHEDULER
