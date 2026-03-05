// include/beman/execution/detail/inline_scheduler.hpp            -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_DETAIL_EXECUTION_INLINE_SCHEDULER
#define INCLUDED_BEMAN_DETAIL_EXECUTION_INLINE_SCHEDULER

// ----------------------------------------------------------------------------
#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <type_traits>
#include <utility>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail.get_completion_scheduler;
import beman.execution.detail.sender;
import beman.execution.detail.receiver;
import beman.execution.detail.set_value;
import beman.execution.detail.operation_state;
import beman.execution.detail.scheduler;
import beman.execution.detail.scheduler_t;
import beman.execution.detail.completion_signatures;
#else
#include <beman/execution/detail/get_completion_scheduler.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/receiver.hpp>
#include <beman/execution/detail/set_value.hpp>
#include <beman/execution/detail/operation_state.hpp>
#include <beman/execution/detail/scheduler.hpp>
#include <beman/execution/detail/completion_signatures.hpp>
#endif

namespace beman::execution {
struct inline_scheduler {
    using scheduler_concept = ::beman::execution::scheduler_t;

    struct env {
        static auto
        query(const ::beman::execution::get_completion_scheduler_t<::beman::execution::set_value_t>&) noexcept
            -> inline_scheduler {
            return {};
        }
    };

    template <::beman::execution::receiver Rcvr>
    struct state {
        using operation_state_concept = ::beman::execution::operation_state_t;
        ::std::remove_cvref_t<Rcvr> rcvr;
        auto                        start() & noexcept -> void { ::beman::execution::set_value(::std::move(rcvr)); }
    };

    struct sender {
        using sender_concept        = ::beman::execution::sender_t;
        using completion_signatures = ::beman::execution::completion_signatures<::beman::execution::set_value_t()>;

        static constexpr auto get_env() noexcept -> env { return {}; }

        template <typename, typename...>
        static consteval auto get_completion_signatures() noexcept -> completion_signatures {
            return {};
        }

        template <::beman::execution::receiver Rcvr>
        auto connect(Rcvr&& receiver) noexcept(::std::is_nothrow_constructible_v<::std::remove_cvref_t<Rcvr>, Rcvr>)
            -> state<Rcvr> {
            return {::std::forward<Rcvr>(receiver)};
        }
    };

    static constexpr auto schedule() noexcept -> sender { return {}; }

    auto operator==(const inline_scheduler&) const -> bool = default;
};
} // namespace beman::execution

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_DETAIL_EXECUTION_INLINE_SCHEDULER
