// include/beman/execution/execution.hpp -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_EXECUTION
#define INCLUDED_BEMAN_EXECUTION_EXECUTION

// ----------------------------------------------------------------------------

#include <beman/execution/detail/queryable.hpp>
#include <beman/execution/detail/env.hpp>
#include <beman/execution/detail/forwarding_query.hpp>
#include <beman/execution/detail/get_allocator.hpp>
#include <beman/execution/detail/get_stop_token.hpp>
#include <beman/execution/detail/get_env.hpp>
#include <beman/execution/detail/get_domain.hpp>
#include <beman/execution/detail/get_scheduler.hpp>
#include <beman/execution/detail/get_completion_scheduler.hpp>
#include <beman/execution/detail/get_delegation_scheduler.hpp>
#include <beman/execution/detail/get_completion_signatures.hpp>
#include <beman/execution/detail/operation_state.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/sender_in.hpp>
#include <beman/execution/detail/scheduler.hpp>
#include <beman/execution/detail/receiver.hpp>
#include <beman/execution/detail/valid_completion_for.hpp>

#include <beman/execution/detail/completion_signature.hpp>
#include <beman/execution/detail/completion_signatures.hpp>
#include <beman/execution/detail/valid_completion_signatures.hpp>
#include <beman/execution/detail/movable_value.hpp>
#include <beman/execution/detail/matching_sig.hpp>
#include <beman/execution/detail/as_except_ptr.hpp>

#include <beman/execution/detail/set_value.hpp>
#include <beman/execution/detail/set_error.hpp>
#include <beman/execution/detail/set_stopped.hpp>
#include <beman/execution/detail/start.hpp>
#include <beman/execution/detail/connect.hpp>
#include <beman/execution/detail/schedule.hpp>

#include <beman/execution/detail/bulk.hpp>
#include <beman/execution/detail/continues_on.hpp>
#include <beman/execution/detail/into_variant.hpp>
#include <beman/execution/detail/just.hpp>
#include <beman/execution/detail/let.hpp>
#include <beman/execution/detail/on.hpp>
#include <beman/execution/detail/prop.hpp>
#include <beman/execution/detail/read_env.hpp>
#include <beman/execution/detail/schedule_from.hpp>
#include <beman/execution/detail/starts_on.hpp>
#include <beman/execution/detail/sync_wait.hpp>
#include <beman/execution/detail/then.hpp>
#include <beman/execution/detail/when_all.hpp>
#include <beman/execution/detail/when_all_with_variant.hpp>
#include <beman/execution/detail/with_awaitable_senders.hpp>
#include <beman/execution/detail/write_env.hpp>

#include <beman/execution/detail/associate.hpp>
#include <beman/execution/detail/counting_scope.hpp>
#include <beman/execution/detail/scope_token.hpp>
#include <beman/execution/detail/simple_counting_scope.hpp>
#include <beman/execution/detail/spawn_future.hpp>
#include <beman/execution/detail/spawn.hpp>

// ----------------------------------------------------------------------------

#endif
