// include/beman/execution/detail/starts_on.hpp                     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_STARTS_ON
#define INCLUDED_BEMAN_EXECUTION_DETAIL_STARTS_ON

#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <concepts>
#include <utility>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail.continues_on;
import beman.execution.detail.default_domain;
import beman.execution.detail.default_impls;
import beman.execution.detail.forward_like;
import beman.execution.detail.fwd_env;
import beman.execution.detail.get_completion_domain;
import beman.execution.detail.get_domain;
import beman.execution.detail.get_env;
import beman.execution.detail.join_env;
import beman.execution.detail.just;
import beman.execution.detail.let;
import beman.execution.detail.make_sender;
import beman.execution.detail.query_with_default;
import beman.execution.detail.sched_env;
import beman.execution.detail.schedule;
import beman.execution.detail.scheduler;
import beman.execution.detail.sender;
import beman.execution.detail.sender_for;
import beman.execution.detail.set_value;
import beman.execution.detail.transform_sender;
#else
#include <beman/execution/detail/continues_on.hpp>
#include <beman/execution/detail/default_domain.hpp>
#include <beman/execution/detail/default_impls.hpp>
#include <beman/execution/detail/forward_like.hpp>
#include <beman/execution/detail/fwd_env.hpp>
#include <beman/execution/detail/get_completion_domain.hpp>
#include <beman/execution/detail/get_domain.hpp>
#include <beman/execution/detail/get_env.hpp>
#include <beman/execution/detail/continues_on.hpp>
#include <beman/execution/detail/join_env.hpp>
#include <beman/execution/detail/just.hpp>
#include <beman/execution/detail/let.hpp>
#include <beman/execution/detail/make_sender.hpp>
#include <beman/execution/detail/query_with_default.hpp>
#include <beman/execution/detail/sched_env.hpp>
#include <beman/execution/detail/schedule.hpp>
#include <beman/execution/detail/scheduler.hpp>
#include <beman/execution/detail/sender_for.hpp>
#include <beman/execution/detail/set_value.hpp>
#include <beman/execution/detail/transform_sender.hpp>
#endif

// ----------------------------------------------------------------------------

namespace beman::execution::detail {

template <typename Scheduler, typename ChildEnv>
struct starts_on_attrs_t {
    Scheduler sch;
    ChildEnv  child_env;

    template <typename Tag, typename Env>
    auto query(::beman::execution::get_completion_domain_t<Tag>, const Env& rcvr_env) const noexcept {
        auto env_for_child = ::beman::execution::detail::join_env(::beman::execution::detail::sched_env(sch),
                                                                  ::beman::execution::detail::fwd_env(rcvr_env));
        if constexpr (requires { ::beman::execution::get_completion_domain<Tag>(child_env, env_for_child); }) {
            return ::beman::execution::get_completion_domain<Tag>(child_env, env_for_child);
        } else {
            return ::beman::execution::default_domain{};
        }
    }

    template <typename Q, typename... Args>
        requires requires { ::std::as_const(child_env).query(::std::declval<Q>(), ::std::declval<Args>()...); }
    auto query(Q q, Args&&... args) const noexcept {
        return ::std::as_const(child_env).query(q, ::std::forward<Args>(args)...);
    }
};

struct starts_on_t {
    template <::beman::execution::detail::sender_for<::beman::execution::detail::starts_on_t> Sender, typename Env>
    auto transform_sender(::beman::execution::set_value_t, Sender&& sender, const Env&) const noexcept {
        auto&& scheduler{sender.template get<1>()};
        auto&& new_sender{sender.template get<2>()};
        return ::beman::execution::let_value(
            ::beman::execution::continues_on(::beman::execution::just(), scheduler),
            [new_sender = ::beman::execution::detail::forward_like<Sender>(new_sender)]() mutable noexcept(
                ::std::is_nothrow_move_constructible_v<::std::remove_cvref_t<Sender>>) {
                return ::std::move(new_sender);
            });
    }

    struct impls_for : ::beman::execution::detail::default_impls {
        struct get_attrs_impl {
            auto operator()(const auto& sch, const auto& child) const noexcept {
                using Sch      = ::std::remove_cvref_t<decltype(sch)>;
                using ChildEnv = ::std::remove_cvref_t<decltype(::beman::execution::get_env(child))>;
                return ::beman::execution::detail::starts_on_attrs_t<Sch, ChildEnv>{
                    sch, ::beman::execution::get_env(child)};
            }
        };
        static constexpr auto get_attrs{get_attrs_impl{}};
    };

    template <::beman::execution::scheduler Scheduler, ::beman::execution::sender Sender>
    auto operator()(Scheduler&& scheduler, Sender&& sender) const {
        return ::beman::execution::detail::make_sender(
            *this, ::std::forward<Scheduler>(scheduler), ::std::forward<Sender>(sender));
    }
};
} // namespace beman::execution::detail

namespace beman::execution {
using starts_on_t = ::beman::execution::detail::starts_on_t;
inline constexpr ::beman::execution::detail::starts_on_t starts_on{};
} // namespace beman::execution

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_STARTS_ON
