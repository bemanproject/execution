// include/beman/execution/detail/task/infallible_scheduler.hpp             -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_TASK_INFALLIBLE_SCHEDULER
#define INCLUDED_BEMAN_EXECUTION_DETAIL_TASK_INFALLIBLE_SCHEDULER

#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <concepts>
#include <utility>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail.completion_signatures;
import beman.execution.detail.completion_signatures_of_t;
import beman.execution.detail.env;
import beman.execution.detail.schedule;
import beman.execution.detail.scheduler;
import beman.execution.detail.set_stopped;
import beman.execution.detail.set_value;
import beman.execution.detail.stop_token_of_t;
import beman.execution.detail.unstoppable_token;
#else
#include <beman/execution/detail/completion_signatures.hpp>
#include <beman/execution/detail/completion_signatures_of_t.hpp>
#include <beman/execution/detail/env.hpp>
#include <beman/execution/detail/schedule.hpp>
#include <beman/execution/detail/scheduler.hpp>
#include <beman/execution/detail/set_stopped.hpp>
#include <beman/execution/detail/set_value.hpp>
#include <beman/execution/detail/stop_token_of_t.hpp>
#include <beman/execution/detail/unstoppable_token.hpp>
#endif

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
template <class Sch, class Env, class... Comp>
concept completes_with =
    ::std::same_as<::beman::execution::completion_signatures<Comp...>,
                   ::beman::execution::
                       completion_signatures_of_t<decltype(::beman::execution::schedule(::std::declval<Sch>())), Env>>;

template <class Sch, class Env>
concept infallible_scheduler =
    (::beman::execution::scheduler<Sch>) &&
    (::beman::execution::detail::completes_with<Sch, Env, ::beman::execution::set_value_t()> ||
     (!::beman::execution::unstoppable_token<::beman::execution::stop_token_of_t<Env>> &&
      (::beman::execution::detail::
           completes_with<Sch, Env, ::beman::execution::set_value_t(), ::beman::execution::set_stopped_t()> ||
       ::beman::execution::detail::
           completes_with<Sch, Env, ::beman::execution::set_stopped_t(), ::beman::execution::set_value_t()>)));
} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif
