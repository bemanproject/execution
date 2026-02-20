module;
// src/beman/execution/execution.cppm
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

//-dk:TODO #include <cstddef>
//-dk:TODO #include <tuple>

export module beman.execution;

import beman.execution.detail.affine_on;
import beman.execution.detail.apply_sender;
import beman.execution.detail.as_awaitable;
export import beman.execution.detail.associate; // [exec.associate]
import beman.execution.detail.bulk;
import beman.execution.detail.check_type_alias_exist;
export import beman.execution.detail.completion_signatures;      // [exec.util.cmplsig]
export import beman.execution.detail.completion_signatures_of_t; // [exec.getcomplsigs], completion signatures
import beman.execution.detail.connect;
export import beman.execution.detail.connect_result_t; // [exec.connect], the connect sender algorithm
import beman.execution.detail.continues_on;
import beman.execution.detail.counting_scope;
import beman.execution.detail.default_domain;
export import beman.execution.detail.env;
export import beman.execution.detail.env_of_t;
export import beman.execution.detail.error_types_of_t; // [exec.getcomplsigs], completion signatures
import beman.execution.detail.forwarding_query;
import beman.execution.detail.get_allocator;
export import beman.execution.detail.get_completion_scheduler;
export import beman.execution.detail.get_completion_signatures; // [exec.getcomplsigs], completion signatures
import beman.execution.detail.get_delegation_scheduler;
import beman.execution.detail.get_domain;
import beman.execution.detail.get_env;
import beman.execution.detail.get_scheduler;
import beman.execution.detail.get_stop_token;
export import beman.execution.detail.inplace_stop_source; // [stopsource.inplace], class inplace_stop_source
import beman.execution.detail.into_variant;
export import beman.execution.detail.just;
import beman.execution.detail.let;
import beman.execution.detail.never_stop_token;
import beman.execution.detail.nostopstate;
import beman.execution.detail.on;
export import beman.execution.detail.operation_state; // [exec.opstate], operation states
import beman.execution.detail.prop;
import beman.execution.detail.read_env;
import beman.execution.detail.run_loop;
import beman.execution.detail.schedule;
import beman.execution.detail.schedule_from;
export import beman.execution.detail.schedule_result_t;
export import beman.execution.detail.scheduler; // [exec.sched], schedulers
export import beman.execution.detail.scheduler_t;
export import beman.execution.detail.scope_association; // [exec.scope.concepts]
export import beman.execution.detail.scope_token;       // [exec.scope.concepts]
import beman.execution.detail.sender_adaptor_closure;
import beman.execution.detail.set_error;
import beman.execution.detail.set_stopped;
import beman.execution.detail.set_value;
import beman.execution.detail.simple_counting_scope;
import beman.execution.detail.spawn;
import beman.execution.detail.spawn_future;
//-dk:TODO import beman.execution.detail.split;
import beman.execution.detail.start;
import beman.execution.detail.starts_on;
export import beman.execution.detail.stop_callback_for_t;
export import beman.execution.detail.stop_source; // [stopsource], class stop_source
import beman.execution.detail.stop_token_of_t;
import beman.execution.detail.stoppable_source;
import beman.execution.detail.sync_wait;
export import beman.execution.detail.tag_of_t; // [exec.getcomplsigs], completion signatures
import beman.execution.detail.then;
import beman.execution.detail.transform_sender;
import beman.execution.detail.valid_completion_for;
export import beman.execution.detail.value_types_of_t; // [exec.getcomplsigs], completion signatures
import beman.execution.detail.when_all;
import beman.execution.detail.when_all_with_variant;
export import beman.execution.detail.with_awaitable_senders; // [exec.with.awaitable.senders]
import beman.execution.detail.write_env;

// [stoptoken.concepts], stop token concepts
export import beman.execution.detail.stoppable_token;
export import beman.execution.detail.unstoppable_token;

// [exec.recv], receivers
export import beman.execution.detail.receiver;
export import beman.execution.detail.receiver_of;

// [exec.snd], senders
export import beman.execution.detail.sender;
export import beman.execution.detail.sender_in;
//-dk:TODO export using ::beman::execution::sender_to;

export import beman.execution.detail.sends_stopped;

namespace beman::execution {

export using ::beman::execution::nostopstate_t;
export using ::beman::execution::nostopstate;

// [stoptoken.never], class never_stop_token
export using ::beman::execution::never_stop_token;

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

export using ::beman::execution::get_domain;
export using ::beman::execution::get_scheduler;
export using ::beman::execution::get_delegation_scheduler;
//-dk:TODO export using ::beman::execution::forward_progress_guarantee;
//-dk:TODO export using ::beman::execution::get_forward_progress_guarantee;

export using ::beman::execution::get_env_t;
export using ::beman::execution::get_env;

// [exec.domain.default], execution_domains
export using ::beman::execution::default_domain;

export using ::beman::execution::set_value_t;
export using ::beman::execution::set_error_t;
export using ::beman::execution::set_stopped_t;

export using ::beman::execution::set_value;
export using ::beman::execution::set_error;
export using ::beman::execution::set_stopped;

// [exec.opstate], operation states
export using ::beman::execution::start_t;
export using ::beman::execution::start;

// [exec.snd.transform], sender transformations
export using ::beman::execution::transform_sender;

// [exec.snd.transform.env], environment transformations
//-dk:TODO export using ::beman::execution::transform_env;

// [exec.snd.apply], sender algorithm application
export using ::beman::execution::apply_sender;

// [exec.connect], the connect sender algorithm
export using ::beman::execution::connect_t;
export using ::beman::execution::connect;

// [exec.factories], sender factories
export using ::beman::execution::schedule_t;

export using ::beman::execution::schedule;
export using ::beman::execution::read_env;

// [exec.adapt], sender adaptors
export using ::beman::execution::sender_adaptor_closure;

export using ::beman::execution::starts_on_t;
export using ::beman::execution::continues_on_t;
export using ::beman::execution::on_t;
export using ::beman::execution::schedule_from_t;
export using ::beman::execution::then_t;
export using ::beman::execution::upon_error_t;
export using ::beman::execution::upon_stopped_t;
export using ::beman::execution::let_value_t;
export using ::beman::execution::let_error_t;
export using ::beman::execution::let_stopped_t;
export using ::beman::execution::bulk_t;
//-dk:TODO export using ::beman::execution::split_t;
export using ::beman::execution::when_all_t;
export using ::beman::execution::when_all_with_variant_t;
export using ::beman::execution::into_variant_t;
//-dk:TODO export using ::beman::execution::stopped_as_optional_t;
//-dk:TODO export using ::beman::execution::stopped_as_error_t;

export using ::beman::execution::starts_on;
export using ::beman::execution::continues_on;
export using ::beman::execution::on;
export using ::beman::execution::schedule_from;
export using ::beman::execution::then;
export using ::beman::execution::upon_error;
export using ::beman::execution::upon_stopped;
export using ::beman::execution::let_value;
export using ::beman::execution::let_error;
export using ::beman::execution::let_stopped;
export using ::beman::execution::bulk;
//-dk:TODO export using ::beman::execution::split;
export using ::beman::execution::when_all;
export using ::beman::execution::when_all_with_variant;
export using ::beman::execution::into_variant;
//-dk:TODO export using ::beman::execution::stopped_as_optional;
//-dk:TODO export using ::beman::execution::stopped_as_error;

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
export using ::beman::execution::as_awaitable_t;
export using ::beman::execution::as_awaitable;

//-dk:TODO add section
export using ::beman::execution::prop;
export using ::beman::execution::write_env_t;
export using ::beman::execution::write_env;
export using ::beman::execution::affine_on_t;
export using ::beman::execution::affine_on;
export using ::beman::execution::read_env_t;
export using ::beman::execution::read_env;
export using ::beman::execution::simple_counting_scope;
export using ::beman::execution::counting_scope;

// [exec.spawn]
export using ::beman::execution::spawn;
export using ::beman::execution::spawn_future;

#if 0
namespace detail {
export using ::beman::execution::detail::await_result_type;
export using ::beman::execution::detail::basic_sender;
export using ::beman::execution::detail::connect_all;
export using ::beman::execution::detail::connect_all_t;
export using ::beman::execution::detail::env_promise;
export using ::beman::execution::detail::is_product_type_c;
export using ::beman::execution::detail::product_type;
export using ::beman::execution::detail::product_type_base;
export using ::beman::execution::detail::sync_wait_env;
export using ::beman::execution::detail::sync_wait_receiver;
export using ::beman::execution::detail::sync_wait_result_type;
export using ::beman::execution::detail::sync_wait_state;
} // namespace detail
#endif

} // namespace beman::execution

#if 0
namespace std {
export template <typename T>
    requires ::beman::execution::detail::is_product_type_c<T>
struct tuple_size<T>;

export template <::std::size_t I, typename T>
    requires ::beman::execution::detail::is_product_type_c<T>
struct tuple_element<I, T>;
} // namespace std
#endif
