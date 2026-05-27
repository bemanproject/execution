// include/beman/execution/detail/get_completion_scheduler.hpp      -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_GET_COMPLETION_SCHEDULER
#define INCLUDED_BEMAN_EXECUTION_DETAIL_GET_COMPLETION_SCHEDULER

#include <beman/execution/detail/common.hpp>
#include <beman/execution/detail/suppress_push.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <concepts>
#include <type_traits>
#include <utility>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail.completion_tag;
import beman.execution.detail.forwarding_query;
import beman.execution.detail.scheduler;
import beman.execution.detail.set_value;
#else
#include <beman/execution/detail/completion_tag.hpp>
#include <beman/execution/detail/forwarding_query.hpp>
#include <beman/execution/detail/scheduler.hpp>
#include <beman/execution/detail/set_value.hpp>
#endif

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
template <typename Scheduler, typename... Envs>
auto recurse_query(Scheduler sch, const Envs&... envs) noexcept;

template <::beman::execution::detail::completion_tag Tag>
struct get_completion_scheduler_t;

template <typename Tag, typename Q, typename... E>
concept compl_sched_recurse_queryable = requires {
    {
        ::beman::execution::detail::recurse_query(
            ::std::declval<const Q&>().query(
                ::std::declval<::beman::execution::detail::get_completion_scheduler_t<Tag>>(),
                ::std::declval<const E&>()...),
            ::std::declval<const E&>()...)
    } -> ::beman::execution::scheduler;
};

template <::beman::execution::detail::completion_tag Tag>
struct get_completion_scheduler_t : ::beman::execution::forwarding_query_t {
    template <typename Q, typename... E>
        requires ::beman::execution::detail::compl_sched_recurse_queryable<Tag, Q, E...> ||
                 (::beman::execution::scheduler<Q> && sizeof...(E) > 0)
    auto operator()(const Q& q, const E&... e) const noexcept;
};

template <typename Scheduler, typename... Envs>
auto recurse_query(Scheduler sch, const Envs&... envs) noexcept {
    ::beman::execution::detail::get_completion_scheduler_t<::beman::execution::set_value_t> get_compl_sch{};
    if constexpr (requires { ::std::as_const(sch).query(get_compl_sch, envs...); }) {
        auto sch2 = ::std::as_const(sch).query(get_compl_sch, envs...);
        if constexpr (::std::same_as<Scheduler, decltype(sch2)>) {
            while (sch != sch2) {
                sch = ::std::exchange(sch2, ::std::as_const(sch2).query(get_compl_sch, envs...));
            }
            return sch;
        } else {
            return (recurse_query)(sch2, envs...);
        }
    } else {
        return sch;
    }
}

template <::beman::execution::detail::completion_tag Tag>
template <typename Q, typename... E>
    requires ::beman::execution::detail::compl_sched_recurse_queryable<Tag, Q, E...> ||
             (::beman::execution::scheduler<Q> && sizeof...(E) > 0)
auto get_completion_scheduler_t<Tag>::operator()(const Q& q, const E&... e) const noexcept {
    if constexpr (::beman::execution::detail::compl_sched_recurse_queryable<Tag, Q, E...>) {
        return ::beman::execution::detail::recurse_query(q.query(*this, e...), e...);
    } else {
        static_assert(::beman::execution::scheduler<Q> && sizeof...(E) > 0);
        return q;
    }
}

} // namespace beman::execution::detail

namespace beman::execution {
template <::beman::execution::detail::completion_tag Tag>
using get_completion_scheduler_t = ::beman::execution::detail::get_completion_scheduler_t<Tag>;
template <::beman::execution::detail::completion_tag Tag>
inline constexpr get_completion_scheduler_t<Tag> get_completion_scheduler{};
} // namespace beman::execution

// ----------------------------------------------------------------------------

#include <beman/execution/detail/suppress_pop.hpp>

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_GET_COMPLETION_SCHEDULER
