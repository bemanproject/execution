// include/beman/execution/detail/starts_on.hpp                     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_STARTS_ON
#define INCLUDED_BEMAN_EXECUTION_DETAIL_STARTS_ON

#include <beman/execution/detail/config.hpp>
#include <beman/execution/detail/scheduler.hpp>
#include <beman/execution/detail/sender_for.hpp>
#include <beman/execution/detail/transform_sender.hpp>
#include <beman/execution/detail/query_with_default.hpp>
#include <beman/execution/detail/make_sender.hpp>
#include <beman/execution/detail/get_domain.hpp>
#include <beman/execution/detail/default_domain.hpp>
#include <beman/execution/detail/join_env.hpp>
#include <beman/execution/detail/sched_env.hpp>
#include <beman/execution/detail/fwd_env.hpp>
#include <beman/execution/detail/forward_like.hpp>
#include <beman/execution/detail/let.hpp>
#include <beman/execution/detail/schedule.hpp>
#include <concepts>
#include <utility>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
struct starts_on_t {
    template <::beman::execution::detail::sender_for<::beman::execution::detail::starts_on_t> Sender, typename Env>
    auto transform_env(Sender&& sender, Env&& env) const noexcept {
        auto&& scheduler{sender.template get<1>()};
        return ::beman::execution::detail::join_env(::beman::execution::detail::sched_env(scheduler),
                                                    ::beman::execution::detail::fwd_env(env));
    }
    template <::beman::execution::detail::sender_for<::beman::execution::detail::starts_on_t> Sender, typename... Env>
    auto transform_sender(Sender&& sender, Env&&...) const noexcept {
        auto&& scheduler{sender.template get<1>()};
        auto&& new_sender{sender.template get<2>()};
        return ::beman::execution::let_value(
            ::beman::execution::schedule(scheduler),
            [new_sender = ::beman::execution::detail::forward_like<Sender>(new_sender)]() mutable noexcept(
                ::std::is_nothrow_constructible_v<::std::decay_t<Sender>>) { return ::std::move(new_sender); });
    }

    template <::beman::execution::scheduler Scheduler, ::beman::execution::sender Sender>
    auto operator()(Scheduler&& scheduler, Sender&& sender) const {
        auto domain{::beman::execution::detail::query_with_default(
            ::beman::execution::get_domain, scheduler, ::beman::execution::default_domain{})};
        return ::beman::execution::transform_sender(
            domain,
            ::beman::execution::detail::make_sender(
                *this, ::std::forward<Scheduler>(scheduler), ::std::forward<Sender>(sender)));
    }
};
} // namespace beman::execution::detail

namespace beman::execution {
BEMAN_EXECUTION_EXPORT using starts_on_t = ::beman::execution::detail::starts_on_t;
BEMAN_EXECUTION_EXPORT inline constexpr ::beman::execution::detail::starts_on_t starts_on{};
} // namespace beman::execution

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_STARTS_ON
