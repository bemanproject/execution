module;
// src/beman/execution/execution.cppm
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_HAS_MODULES
#define BEMAN_HAS_MODULES
#endif
// #include <beman/execution/execution.hpp>
// #include <beman/execution/stop_token.hpp>

export module beman.execution;

export import beman.execution.detail.affine_on;
export import beman.execution.detail.bulk;
export import beman.execution.detail.check_type_alias_exist;
export import beman.execution.detail.completion_signatures;
export import beman.execution.detail.completion_signatures_of_t;
export import beman.execution.detail.connect;
export import beman.execution.detail.connect_result_t;
export import beman.execution.detail.default_domain;
export import beman.execution.detail.env;
export import beman.execution.detail.env_of_t;
export import beman.execution.detail.forwarding_query;
export import beman.execution.detail.get_allocator;
export import beman.execution.detail.get_completion_scheduler;
export import beman.execution.detail.get_completion_signatures;
export import beman.execution.detail.get_delegation_scheduler;
export import beman.execution.detail.get_domain;
export import beman.execution.detail.get_env;
export import beman.execution.detail.get_scheduler;
export import beman.execution.detail.get_stop_token;
export import beman.execution.detail.inplace_stop_source;
export import beman.execution.detail.just;
export import beman.execution.detail.never_stop_token;
export import beman.execution.detail.nostopstate;
export import beman.execution.detail.operation_state;
export import beman.execution.detail.prop;
export import beman.execution.detail.read_env;
export import beman.execution.detail.receiver;
export import beman.execution.detail.receiver_of;
export import beman.execution.detail.run_loop;
export import beman.execution.detail.schedule;
export import beman.execution.detail.scheduler;
export import beman.execution.detail.scheduler_t;
export import beman.execution.detail.sender;
export import beman.execution.detail.sender_in;
export import beman.execution.detail.sends_stopped;
export import beman.execution.detail.set_error;
export import beman.execution.detail.set_stopped;
export import beman.execution.detail.set_value;
export import beman.execution.detail.start;
export import beman.execution.detail.starts_on;
export import beman.execution.detail.stop_callback_for_t;
export import beman.execution.detail.stop_source;
export import beman.execution.detail.stop_token_of_t;
export import beman.execution.detail.stoppable_source;
export import beman.execution.detail.stoppable_token;
export import beman.execution.detail.sync_wait;
export import beman.execution.detail.tag_of_t;
export import beman.execution.detail.then;
export import beman.execution.detail.transform_sender;
export import beman.execution.detail.unstoppable_token;
export import beman.execution.detail.valid_completion_for;
export import beman.execution.detail.write_env;
export import beman.execution.detail.schedule_result_t;
export import beman.execution.detail.sender_adaptor_closure;
export import beman.execution.detail.continues_on;
export import beman.execution.detail.on;
export import beman.execution.detail.schedule_from;
export import beman.execution.detail.let;
//-dk:TODO? export import beman.execution.detail.split;
export import beman.execution.detail.when_all;
export import beman.execution.detail.when_all_with_variant;
export import beman.execution.detail.into_variant;
export import beman.execution.detail.as_awaitable;
export import beman.execution.detail.with_awaitable_senders;
export import beman.execution.detail.value_types_of_t;
export import beman.execution.detail.counting_scope;
export import beman.execution.detail.simple_counting_scope;
export import beman.execution.detail.scope_token;
export import beman.execution.detail.apply_sender;
export import beman.execution.detail.error_types_of_t;
