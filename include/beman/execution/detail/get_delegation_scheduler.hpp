// include/beman/execution/detail/get_delegation_scheduler.hpp      -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_GET_DELEGATION_SCHEDULER
#define INCLUDED_BEMAN_EXECUTION_DETAIL_GET_DELEGATION_SCHEDULER

#include <beman/execution/detail/common.hpp>
#include <beman/execution/detail/forwarding_query.hpp>
#include <beman/execution/detail/scheduler.hpp>
#include <utility>

// ----------------------------------------------------------------------------

namespace beman::execution {
struct get_delegation_scheduler_t {
    template <typename Env>
        requires requires(Env&& env, const get_delegation_scheduler_t& g) {
            { ::std::as_const(env).query(g) } noexcept -> ::beman::execution::scheduler;
        }
    auto operator()(Env&& env) const noexcept {
        return ::std::as_const(env).query(*this);
    }
    constexpr auto query(const ::beman::execution::forwarding_query_t&) const noexcept -> bool { return true; }
};

inline constexpr get_delegation_scheduler_t get_delegation_scheduler{};
} // namespace beman::execution

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_GET_DELEGATION_SCHEDULER
