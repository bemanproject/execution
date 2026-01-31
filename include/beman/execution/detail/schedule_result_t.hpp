// include/beman/execution/detail/schedule_result_t.hpp             -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_SCHEDULE_RESULT
#define INCLUDED_BEMAN_EXECUTION_DETAIL_SCHEDULE_RESULT

#include <beman/execution/detail/common.hpp>
#include <beman/execution/detail/scheduler.hpp>
#include <beman/execution/detail/schedule.hpp>
#include <type_traits>

// ----------------------------------------------------------------------------

namespace beman::execution {
BEMAN_EXECUTION_EXPORT template <::beman::execution::scheduler Scheduler>
using schedule_result_t = decltype(::beman::execution::schedule(::std::declval<Scheduler>()));
}

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_SCHEDULE_RESULT
