// include/beman/execution26/execution.hpp                            -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_EXECUTION
#define INCLUDED_BEMAN_EXECUTION26_EXECUTION

#include <beman/execution/execution.hpp>

// ----------------------------------------------------------------------------

namespace beman::execution26::detail {
using ::beman::execution::detail::make_env;
using ::beman::execution::detail::sender_adaptor;
} // namespace beman::execution26::detail

namespace beman::execution26 {

using ::beman::execution::apply_sender;
using ::beman::execution::as_awaitable;
using ::beman::execution::as_awaitable_t;
using ::beman::execution::completion_signatures;
using ::beman::execution::completion_signatures_of_t;
using ::beman::execution::connect;
using ::beman::execution::connect_result_t;
using ::beman::execution::connect_t;
using ::beman::execution::continues_on;
using ::beman::execution::continues_on_t;
using ::beman::execution::default_domain;
using ::beman::execution::env;
using ::beman::execution::env_of_t;
using ::beman::execution::error_types_of_t;
using ::beman::execution::forwarding_query;
using ::beman::execution::forwarding_query_t;
using ::beman::execution::get_allocator;
using ::beman::execution::get_allocator_t;
using ::beman::execution::get_completion_scheduler;
using ::beman::execution::get_completion_scheduler_t;
using ::beman::execution::get_completion_signatures;
using ::beman::execution::get_completion_signatures_t;
using ::beman::execution::get_delegation_scheduler;
using ::beman::execution::get_delegation_scheduler_t;
using ::beman::execution::get_domain;
using ::beman::execution::get_domain_t;
using ::beman::execution::get_env;
using ::beman::execution::get_env_t;
using ::beman::execution::get_scheduler;
using ::beman::execution::get_scheduler_t;
using ::beman::execution::get_stop_token;
using ::beman::execution::get_stop_token_t;
using ::beman::execution::into_variant;
using ::beman::execution::into_variant_t;
using ::beman::execution::just;
using ::beman::execution::just_error;
using ::beman::execution::just_error_t;
using ::beman::execution::just_stopped;
using ::beman::execution::just_stopped_t;
using ::beman::execution::just_t;
using ::beman::execution::let_error;
using ::beman::execution::let_error_t;
using ::beman::execution::let_stopped;
using ::beman::execution::let_stopped_t;
using ::beman::execution::let_value;
using ::beman::execution::let_value_t;
using ::beman::execution::operation_state;
using ::beman::execution::operation_state_t;
using ::beman::execution::read_env;
using ::beman::execution::receiver;
using ::beman::execution::receiver_of;
using ::beman::execution::receiver_t;
using ::beman::execution::run_loop;
using ::beman::execution::schedule;
using ::beman::execution::schedule_from;
using ::beman::execution::schedule_from_t;
using ::beman::execution::schedule_result_t;
using ::beman::execution::schedule_t;
using ::beman::execution::scheduler;
using ::beman::execution::scheduler_t;
using ::beman::execution::sender;
using ::beman::execution::sender_adaptor_closure;
using ::beman::execution::sender_in;
using ::beman::execution::sender_t;
using ::beman::execution::sends_stopped;
using ::beman::execution::set_error;
using ::beman::execution::set_error_t;
using ::beman::execution::set_stopped;
using ::beman::execution::set_stopped_t;
using ::beman::execution::set_value;
using ::beman::execution::set_value_t;
using ::beman::execution::start;
using ::beman::execution::start_t;
using ::beman::execution::starts_on;
using ::beman::execution::starts_on_t;
using ::beman::execution::stop_token_of_t;
using ::beman::execution::sync_wait;
using ::beman::execution::sync_wait_t;
using ::beman::execution::tag_of_t;
using ::beman::execution::then;
using ::beman::execution::then_t;
using ::beman::execution::transform_sender;
using ::beman::execution::upon_error;
using ::beman::execution::upon_error_t;
using ::beman::execution::upon_stopped;
using ::beman::execution::upon_stopped_t;
using ::beman::execution::value_types_of_t;
using ::beman::execution::when_all;
using ::beman::execution::when_all_t;
using ::beman::execution::when_all_with_variant;
using ::beman::execution::when_all_with_variant_t;
using ::beman::execution::with_awaitable_senders;

} // namespace beman::execution26

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION26_EXECUTION
