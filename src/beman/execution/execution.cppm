// src/beman/execution/execution.cpp -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// ----------------------------------------------------------------------------

module;
#include <execution>
#include <stop_token>
#include <beman/execution/execution.hpp>
#include <beman/execution/stop_token.hpp>

export module beman_execution;

namespace beman::execution {
export int version(0);
// [stoptoken.concepts], stop token concepts
export using ::beman::execution::stoppable_token;
export using ::beman::execution::unstoppable_token;

// [stoptoken], class stop_token
export using ::beman::execution::stop_token;

// [stopsource], class stop_source
export using ::beman::execution::stop_source;

// no-shared-stop-state indicator
//-dk:TODO export using ::beman::execution::no_stop_state_t;

// [stopcallback], class template stop_callback
export using ::beman::execution::stop_callback;

// [stoptoken.never], class never_stop_token
export using ::beman::execution::never_stop_token;

// [stoptoken.inplace], class inplace_stop_token
export using ::beman::execution::inplace_stop_token;

// [stopsource.inplace], class inplace_stop_source
export using ::beman::execution::inplace_stop_source;

// [stopcallback.inplace], class template inplace_stop_callback
export using ::beman::execution::inplace_stop_callback;
export using ::beman::execution::stop_callback_for_t;

#if 0
    //-dk:TODO enable the execution policies
    export using ::std::is_execution_policy;
    export using ::std::is_execution_policy_v;

    export using ::std::execution::sequenced_policy;
    export using ::std::execution::parallel_policy;
    export using ::std::execution::parallel_unsequenced_policy;
    export using ::std::execution::unsequenced_policy;

    export using ::std::execution::seq;
    export using ::std::execution::par;
    export using ::std::execution::par_unseq;
    export using ::std::execution::unseq;
#endif

// [exec.queries], queries
export using ::beman::execution::forwarding_query_t;
export using ::beman::execution::get_allocator_t;
export using ::beman::execution::get_stop_token_t;

export using ::beman::execution::forwarding_query;
export using ::beman::execution::get_allocator;
export using ::beman::execution::get_stop_token;

export using ::beman::execution::stop_token_of_t;

export using ::beman::execution::get_domain_t;
export using ::beman::execution::get_scheduler_t;
export using ::beman::execution::get_delegation_scheduler_t;
//-dk:TODO export using ::beman::execution::get_forward_progress_guarantee_t;
export using ::beman::execution::get_completion_scheduler_t;

export using ::beman::execution::get_domain;
export using ::beman::execution::get_scheduler;
export using ::beman::execution::get_delegation_scheduler;
//-dk:TODO export using ::beman::execution::forward_progress_guarantee;
//-dk:TODO export using ::beman::execution::get_forward_progress_guarantee;
export using ::beman::execution::get_completion_scheduler;

export using ::beman::execution::env;
export using ::beman::execution::get_env_t;
export using ::beman::execution::get_env;

export using ::beman::execution::env_of_t;

// [exec.domain.default], execution_domains
export using ::beman::execution::default_domain;

// [exec.sched], schedulers
export using ::beman::execution::scheduler_t;
export using ::beman::execution::scheduler;

// [exec.recv], receivers
export using ::beman::execution::receiver_t;
export using ::beman::execution::receiver;
export using ::beman::execution::receiver_of;

export using ::beman::execution::set_value_t;
export using ::beman::execution::set_error_t;
export using ::beman::execution::set_stopped_t;

export using ::beman::execution::set_value;
export using ::beman::execution::set_error;
export using ::beman::execution::set_stopped;

// [exec.opstate], operation states
export using ::beman::execution::operation_state_t;
export using ::beman::execution::operation_state;
export using ::beman::execution::start_t;
export using ::beman::execution::start;

// [exec.snd], senders
export using ::beman::execution::sender_t;
export using ::beman::execution::sender;
export using ::beman::execution::sender_in;
//-dk:TODO export using ::beman::execution::sender_to;

// [exec.getcomplsigs], completion signatures
export using ::beman::execution::get_completion_signatures_t;
export using ::beman::execution::get_completion_signatures;
export using ::beman::execution::completion_signatures_of_t;
export using ::beman::execution::value_types_of_t;
export using ::beman::execution::error_types_of_t;
export using ::beman::execution::sends_stopped;
export using ::beman::execution::tag_of_t;

// [exec.snd.transform], sender transformations
export using ::beman::execution::transform_sender;

// [exec.snd.transform.env], environment transformations
//-dk:TODO export using ::beman::execution::transform_env;

// [exec.snd.apply], sender algorithm application
export using ::beman::execution::apply_sender;

// [exec.connect], the connect sender algorithm
export using ::beman::execution::connect_t;
export using ::beman::execution::connect;
export using ::beman::execution::connect_result_t;

// [exec.factories], sender factories
export using ::beman::execution::just_t;
export using ::beman::execution::just_error_t;
export using ::beman::execution::just_stopped_t;
export using ::beman::execution::schedule_t;

export using ::beman::execution::just;
export using ::beman::execution::just_error;
export using ::beman::execution::just_stopped;
export using ::beman::execution::schedule;
export using ::beman::execution::read_env;

export using ::beman::execution::schedule_result_t;

// [exec.adapt], sender adaptors
export using ::beman::execution::sender_adaptor_closure;

export using ::beman::execution::starts_on_t;
export using ::beman::execution::continues_on_t;
//-dk:TODO export using ::beman::execution::on_t;
export using ::beman::execution::schedule_from_t;
export using ::beman::execution::then_t;
export using ::beman::execution::upon_error_t;
export using ::beman::execution::upon_stopped_t;
export using ::beman::execution::let_value_t;
export using ::beman::execution::let_error_t;
export using ::beman::execution::let_stopped_t;
//-dk:TODO export using ::beman::execution::bulk_t;
//-dk:TODO export using ::beman::execution::split_t;
export using ::beman::execution::when_all_t;
export using ::beman::execution::when_all_with_variant_t;
export using ::beman::execution::into_variant_t;
//-dk:TODO export using ::beman::execution::stopped_as_optional_t;
//-dk:TODO export using ::beman::execution::stopped_as_error_t;

export using ::beman::execution::starts_on;
export using ::beman::execution::continues_on;
//-dk:TODO export using ::beman::execution::on;
export using ::beman::execution::schedule_from;
export using ::beman::execution::then;
export using ::beman::execution::upon_error;
export using ::beman::execution::upon_stopped;
export using ::beman::execution::let_value;
export using ::beman::execution::let_error;
export using ::beman::execution::let_stopped;
//-dk:TODO export using ::beman::execution::bulk;
//-dk:TODO export using ::beman::execution::split;
export using ::beman::execution::when_all;
export using ::beman::execution::when_all_with_variant;
export using ::beman::execution::into_variant;
//-dk:TODO export using ::beman::execution::stopped_as_optional;
//-dk:TODO export using ::beman::execution::stopped_as_error;

// [exec.util.cmplsig]
export using ::beman::execution::completion_signatures;

// [exec.util.cmplsig.trans]
//-dk:TODO export using ::beman::execution::transform_completion_signatures;
//-dk:TODO export using ::beman::execution::transform_completion_signatures_of;

// [exec.run.loop], run_loop
export using ::beman::execution::run_loop;

// [exec.consumers], consumers
export using ::beman::execution::sync_wait_t;
//-dk:TODO export using ::beman::execution::sync_wait_with_variant_t;

export using ::beman::execution::sync_wait;
//-dk:TODO export using ::beman::execution::sync_wait_with_variant;

// [exec.as.awaitable]
//-dk:TODO export using ::beman::execution::as_awaitable_t;
//-dk:TODO export using ::beman::execution::as_awaitable;

// [exec.with.awaitable.senders]
//-dk:TODO export using ::beman::execution::with_awaitable_senders;

} // namespace beman::execution
