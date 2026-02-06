module;
// src/beman/execution/execution-detail.cppm                          -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/detail/as_except_ptr.hpp>
#include <beman/execution/detail/await_suspend_result.hpp>
#include <beman/execution/detail/basic_operation.hpp>
#include <beman/execution/detail/basic_receiver.hpp>
#include <beman/execution/detail/basic_sender.hpp>
#include <beman/execution/detail/basic_state.hpp>
#include <beman/execution/detail/call_result_t.hpp>
#include <beman/execution/detail/callable.hpp>
#include <beman/execution/detail/child_type.hpp>
#include <beman/execution/detail/completion_domain.hpp>
#include <beman/execution/detail/completion_signature.hpp>
#include <beman/execution/detail/completion_signatures_for.hpp>
#include <beman/execution/detail/completion_tag.hpp>
#include <beman/execution/detail/connect_all.hpp>
#include <beman/execution/detail/connect_all_result.hpp>
#include <beman/execution/detail/connect_awaitable.hpp>
#include <beman/execution/detail/decayed_tuple.hpp>
#include <beman/execution/detail/decayed_typeof.hpp>
#include <beman/execution/detail/decays_to.hpp>
#include <beman/execution/detail/default_impls.hpp>
#include <beman/execution/detail/env_type.hpp>
#include <beman/execution/detail/forward_like.hpp>
#include <beman/execution/detail/fwd_env.hpp>
#include <beman/execution/detail/gather_signatures.hpp>
#include <beman/execution/detail/get_awaiter.hpp>
#include <beman/execution/detail/get_domain_early.hpp>
#include <beman/execution/detail/get_domain_late.hpp>
#include <beman/execution/detail/has_as_awaitable.hpp>
#include <beman/execution/detail/has_completions.hpp>
#include <beman/execution/detail/immovable.hpp>
#include <beman/execution/detail/impls_for.hpp>
#include <beman/execution/detail/indices_for.hpp>
#include <beman/execution/detail/indirect_meta_apply.hpp>
#include <beman/execution/detail/is_awaitable.hpp>
#include <beman/execution/detail/is_awaiter.hpp>
#include <beman/execution/detail/join_env.hpp>
#include <beman/execution/detail/make_env.hpp>
#include <beman/execution/detail/make_sender.hpp>
#include <beman/execution/detail/matching_sig.hpp>
#include <beman/execution/detail/meta_combine.hpp>
#include <beman/execution/detail/meta_contain_same.hpp>
#include <beman/execution/detail/meta_contains.hpp>
#include <beman/execution/detail/meta_filter.hpp>
#include <beman/execution/detail/meta_prepend.hpp>
#include <beman/execution/detail/meta_transform.hpp>
#include <beman/execution/detail/meta_unique.hpp>
#include <beman/execution/detail/movable_value.hpp>
#include <beman/execution/detail/non_assignable.hpp>
#include <beman/execution/detail/nothrow_callable.hpp>
#include <beman/execution/detail/notify.hpp>
#include <beman/execution/detail/operation_state_task.hpp>
#include <beman/execution/detail/product_type.hpp>
#include <beman/execution/detail/query_with_default.hpp>
#include <beman/execution/detail/queryable.hpp>
#include <beman/execution/detail/sched_attrs.hpp>
#include <beman/execution/detail/sched_env.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/sender_adaptor.hpp>
#include <beman/execution/detail/sender_adaptor_closure.hpp>
#include <beman/execution/detail/sender_for.hpp>
#include <beman/execution/detail/simple_allocator.hpp>
#include <beman/execution/detail/single_sender.hpp>
#include <beman/execution/detail/single_sender_value_type.hpp>
#include <beman/execution/detail/spawn.hpp>
#include <beman/execution/detail/spawn_future.hpp>
#include <beman/execution/detail/state_type.hpp>
#include <beman/execution/detail/stop_callback_for_t.hpp>
#include <beman/execution/detail/stop_when.hpp>
#include <beman/execution/detail/stoppable_source.hpp>
#include <beman/execution/detail/suspend_complete.hpp>
#include <beman/execution/detail/sync_wait.hpp>
#include <beman/execution/detail/type_list.hpp>
#include <beman/execution/detail/valid_completion_for.hpp>
#include <beman/execution/detail/valid_completion_signatures.hpp>
#include <beman/execution/detail/valid_specialization.hpp>
#include <beman/execution/detail/variant_or_empty.hpp>
#include <beman/execution/detail/with_await_transform.hpp>
#include <beman/execution/detail/write_env.hpp>

export module beman.execution.detail;

namespace beman::execution::detail::meta {
export using beman::execution::detail::meta::combine;
export using beman::execution::detail::meta::contain_same;
export using beman::execution::detail::meta::contains;
export using beman::execution::detail::meta::filter;
export using beman::execution::detail::meta::prepend;
export using beman::execution::detail::meta::transform;
export using beman::execution::detail::meta::unique;
} // namespace beman::execution::detail::meta

namespace beman::execution::detail {
export using beman::execution::detail::always_true;
export using beman::execution::detail::as_except_ptr;
export using beman::execution::detail::await_suspend_result;
export using beman::execution::detail::basic_operation;
export using beman::execution::detail::basic_receiver;
export using beman::execution::detail::basic_sender;
export using beman::execution::detail::basic_state;
export using beman::execution::detail::call_result_t;
export using beman::execution::detail::callable;
export using beman::execution::detail::child_type;
export using beman::execution::detail::completion_domain;
export using beman::execution::detail::completion_signature;
export using beman::execution::detail::completion_signatures_for;
export using beman::execution::detail::completion_signatures_for_impl;
export using beman::execution::detail::completion_tag;
export using beman::execution::detail::connect_all;
export using beman::execution::detail::connect_all_result;
export using beman::execution::detail::connect_awaitable;
export using beman::execution::detail::connect_awaitable_promise;
export using beman::execution::detail::decayed_tuple;
export using beman::execution::detail::decayed_typeof;
export using beman::execution::detail::decays_to;
export using beman::execution::detail::default_impls;
export using beman::execution::detail::empty_variant;
export using beman::execution::detail::enable_sender;
export using beman::execution::detail::env_type;
export using beman::execution::detail::forward_like;
export using beman::execution::detail::fwd_env;
export using beman::execution::detail::gather_signatures;
export using beman::execution::detail::get_awaiter;
export using beman::execution::detail::get_domain_early;
export using beman::execution::detail::get_domain_late;
export using beman::execution::detail::get_sender_data;
export using beman::execution::detail::has_as_awaitable;
export using beman::execution::detail::has_completions;
export using beman::execution::detail::immovable;
export using beman::execution::detail::impls_for;
export using beman::execution::detail::indices_for;
export using beman::execution::detail::indirect_meta_apply;
export using beman::execution::detail::is_awaitable;
export using beman::execution::detail::is_awaiter;
export using beman::execution::detail::is_product_type;
export using beman::execution::detail::is_sender;
export using beman::execution::detail::is_sender_adaptor_closure;
export using beman::execution::detail::join_env;
export using beman::execution::detail::make_env;
export using beman::execution::detail::make_sender;
export using beman::execution::detail::matching_sig;
export using beman::execution::detail::movable_value;
export using beman::execution::detail::no_completion_signatures_defined_in_sender;
export using beman::execution::detail::non_assignable;
export using beman::execution::detail::nothrow_callable;
export using beman::execution::detail::notifier;
export using beman::execution::detail::notify;
export using beman::execution::detail::notify_t;
export using beman::execution::detail::operation_state_task;
export using beman::execution::detail::own_forward_like;
export using beman::execution::detail::product_type;
export using beman::execution::detail::query_with_default;
export using beman::execution::detail::queryable;
export using beman::execution::detail::sched_attrs;
export using beman::execution::detail::sched_env;
export using beman::execution::detail::sender_adaptor;
export using beman::execution::detail::sender_for;
export using beman::execution::detail::simple_allocator;
export using beman::execution::detail::single_sender;
export using beman::execution::detail::single_sender_value_type;
export using beman::execution::detail::spawn_future_receiver;
export using beman::execution::detail::spawn_future_state_base;
export using beman::execution::detail::spawn_get_allocator;
export using beman::execution::detail::spawn_t;
export using beman::execution::detail::state_type;
export using beman::execution::detail::stop_when;
export using beman::execution::detail::stop_when_t;
export using beman::execution::detail::stoppable_callback_for;
export using beman::execution::detail::stoppable_source;
export using beman::execution::detail::suspend_complete;
export using beman::execution::detail::sync_wait_env;
export using beman::execution::detail::sync_wait_receiver;
export using beman::execution::detail::sync_wait_result_type;
export using beman::execution::detail::sync_wait_state;
export using beman::execution::detail::type_list;
export using beman::execution::detail::valid_completion_for;
export using beman::execution::detail::valid_completion_signatures;
export using beman::execution::detail::valid_specialization;
export using beman::execution::detail::variant_or_empty;
export using beman::execution::detail::with_await_transform;
export using beman::execution::detail::write_env;
export using beman::execution::detail::write_env_t;

} // namespace beman::execution::detail
