// include/beman/execution/detail/parallel_scheduler.hpp            -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_PARALLEL_SCHEDULER
#define INCLUDED_BEMAN_EXECUTION_DETAIL_PARALLEL_SCHEDULER

#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <cstddef>
#include <concepts>
#include <exception>
#include <memory>
#include <optional>
#include <span>
#include <type_traits>
#include <utility>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail.completion_signatures;
import beman.execution.detail.get_completion_scheduler;
import beman.execution.detail.get_forward_progress_guarantee;
import beman.execution.detail.operation_state;
import beman.execution.detail.receiver;
import beman.execution.detail.scheduler;
import beman.execution.detail.scheduler_t;
import beman.execution.detail.sender;
import beman.execution.detail.set_error;
import beman.execution.detail.set_stopped;
import beman.execution.detail.set_value;
#else
#include <beman/execution/detail/completion_signatures.hpp>
#include <beman/execution/detail/get_completion_scheduler.hpp>
#include <beman/execution/detail/get_forward_progress_guarantee.hpp>
#include <beman/execution/detail/operation_state.hpp>
#include <beman/execution/detail/receiver.hpp>
#include <beman/execution/detail/scheduler.hpp>
#include <beman/execution/detail/scheduler_t.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/set_error.hpp>
#include <beman/execution/detail/set_stopped.hpp>
#include <beman/execution/detail/set_value.hpp>
#endif

// ----------------------------------------------------------------------------

namespace beman::execution::system_context_replaceability {

struct receiver_proxy {
    virtual ~receiver_proxy() = default;

    virtual auto set_value() noexcept -> void                     = 0;
    virtual auto set_error(::std::exception_ptr) noexcept -> void = 0;
    virtual auto set_stopped() noexcept -> void                   = 0;

    template <class P, class Query>
        requires(::std::same_as<P, ::std::remove_cv_t<P>> && ::std::is_object_v<P> && !::std::is_array_v<P>)
    auto try_query(Query) noexcept -> ::std::optional<P> {
        // TODO(P2079R10): forward supported receiver environment queries
        // through this proxy, especially get_stop_token_t -> inplace_stop_token
        return ::std::nullopt;
    }
};

struct bulk_item_receiver_proxy : receiver_proxy {
    virtual auto execute(::std::size_t, ::std::size_t) noexcept -> void = 0;
};

} // namespace beman::execution::system_context_replaceability

namespace beman::execution {

class parallel_scheduler {
    // TODO(P2079R10): add scheduler state and operations.
};

// TODO(P2079R10): implement using system_context_replaceability::query_parallel_scheduler_backend().
auto get_parallel_scheduler() -> parallel_scheduler;

} // namespace beman::execution

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_PARALLEL_SCHEDULER
