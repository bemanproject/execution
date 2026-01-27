// include/beman/execution/detail/scheduler.hpp                     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_SCHEDULER
#define INCLUDED_BEMAN_EXECUTION_DETAIL_SCHEDULER

#include <beman/execution/detail/almost_scheduler.hpp>
#include <beman/execution/detail/decayed_same_as.hpp>
#include <beman/execution/detail/get_completion_scheduler.hpp>
#include <beman/execution/detail/get_env.hpp>
#include <beman/execution/detail/schedule.hpp>
#include <beman/execution/detail/set_value.hpp>
#include <concepts>
#include <type_traits>

// ----------------------------------------------------------------------------

namespace beman::execution {
template <typename Scheduler>
concept scheduler = ::beman::execution::detail::almost_scheduler<Scheduler> && requires(Scheduler&& sched) {
    {
        ::beman::execution::get_completion_scheduler<::beman::execution::set_value_t>(
            ::beman::execution::get_env(::beman::execution::schedule(::std::forward<Scheduler>(sched))))
    } -> ::beman::execution::detail::decayed_same_as<Scheduler>;
};
} // namespace beman::execution

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_SCHEDULER
