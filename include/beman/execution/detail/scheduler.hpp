// include/beman/execution/detail/scheduler.hpp                     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_SCHEDULER
#define INCLUDED_BEMAN_EXECUTION_DETAIL_SCHEDULER

#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <concepts>
#include <type_traits>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail.almost_scheduler import beman.execution.detail.decayed_same_as import beman.execution.detail.get_completion_scheduler import beman.execution.detail.get_env import beman.execution.detail.schedule import beman.execution.detail.set_value
#else
#include <beman/execution/detail/almost_scheduler.hpp>
#include <beman/execution/detail/decayed_same_as.hpp>
#include <beman/execution/detail/get_completion_scheduler.hpp>
#include <beman/execution/detail/get_env.hpp>
#include <beman/execution/detail/schedule.hpp>
#include <beman/execution/detail/set_value.hpp>
#endif

    // ----------------------------------------------------------------------------

    namespace beman::execution { template <typename Scheduler> concept scheduler = ::beman::execution::detail::almost_scheduler <Scheduler> && requires(Scheduler && sched) { { ::beman::execution::get_completion_scheduler <::beman::execution::set_value_t>(::beman::execution::get_env(::beman::execution::schedule(::std::forward <Scheduler>(sched)))) }->::beman::execution::detail::decayed_same_as <Scheduler>;
};
} // namespace beman::execution

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_SCHEDULER
