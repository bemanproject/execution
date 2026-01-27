// include/beman/execution/detail/almost_scheduler.hpp              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_ALMOST_SCHEDULER
#define INCLUDED_BEMAN_EXECUTION_DETAIL_ALMOST_SCHEDULER

#include <beman/execution/detail/common.hpp>
#include <beman/execution/detail/queryable.hpp>
#include <beman/execution/detail/schedule.hpp>
#include <beman/execution/detail/scheduler_t.hpp>
#include <beman/execution/detail/sender.hpp>
#include <concepts>
#include <utility>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
/*!
 * \brief Auxiliary concept used to break cycle for scheduler concept.
 * \headerfile beman/execution/execution.hpp <beman/execution/execution.hpp>
 * \internal
 * \concept almost_scheduler
 */
template <typename Scheduler>
concept almost_scheduler = ::std::derived_from<typename ::std::remove_cvref_t<Scheduler>::scheduler_concept,
                                               ::beman::execution::scheduler_t> &&
                           ::beman::execution::detail::queryable<Scheduler> &&
                           requires(Scheduler&& sched) {
                               {
                                   ::beman::execution::schedule(::std::forward<Scheduler>(sched))
                               } -> ::beman::execution::sender;
                           } && ::std::equality_comparable<::std::remove_cvref_t<Scheduler>> &&
                           ::std::copy_constructible<::std::remove_cvref_t<Scheduler>>;
} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_ALMOST_SCHEDULER
