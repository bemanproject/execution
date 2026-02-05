// include/beman/execution/detail/on.hpp                            -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_ON
#define INCLUDED_BEMAN_EXECUTION_DETAIL_ON

#include <beman/execution/detail/common.hpp>
#include <beman/execution/detail/scheduler.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/sender_adaptor_closure.hpp>
#include <beman/execution/detail/transform_sender.hpp>
#include <beman/execution/detail/query_with_default.hpp>
#include <beman/execution/detail/get_domain.hpp>
#include <beman/execution/detail/get_domain_early.hpp>
#include <beman/execution/detail/default_domain.hpp>
#include <beman/execution/detail/make_sender.hpp>
#include <beman/execution/detail/product_type.hpp>
#include <beman/execution/detail/sender_for.hpp>
#include <beman/execution/detail/join_env.hpp>
#include <beman/execution/detail/forward_like.hpp>
#include <beman/execution/detail/fwd_env.hpp>
#include <beman/execution/detail/sched_env.hpp>
#include <beman/execution/detail/starts_on.hpp>
#include <beman/execution/detail/continues_on.hpp>
#include <beman/execution/detail/write_env.hpp>
#include <utility>

#include <beman/execution/detail/suppress_push.hpp>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
struct on_t : ::beman::execution::sender_adaptor_closure<on_t> {
    template <::beman::execution::detail::sender_for<on_t> OutSndr, typename Env>
    auto transform_env(OutSndr&& out_sndr, Env&& env) const -> decltype(auto) {
        auto&& data{out_sndr.template get<1>()};

        if constexpr (::beman::execution::scheduler<decltype(data)>)
            return ::beman::execution::detail::join_env(
                ::beman::execution::detail::sched_env(::beman::execution::detail::forward_like<OutSndr>(data)

                                                          ),
                ::beman::execution::detail::fwd_env(::std::forward<Env>(env)));
        else
            return std::forward<Env>(env);
    }

    template <typename>
    struct env_needs_get_scheduler {
        using sender_concept = ::beman::execution::sender_t;
        template <typename Env>
        auto get_completion_signatures(Env&&) const {
            return env_needs_get_scheduler<Env>{};
        }
    };

    template <::beman::execution::detail::sender_for<on_t> OutSndr, typename Env>
    auto transform_sender(OutSndr&& out_sndr, Env&& env) const -> decltype(auto) {
        struct not_a_scheduler {};
        auto&& [_, data, child] = out_sndr;

        if constexpr (::beman::execution::scheduler<decltype(data)>) {
            auto sch{::beman::execution::detail::query_with_default(
                ::beman::execution::get_scheduler, env, not_a_scheduler{})};
            if constexpr (::std::same_as<not_a_scheduler, decltype(sch)>) {
                return env_needs_get_scheduler<Env>{};
            } else {
                return ::beman::execution::continues_on(
                    ::beman::execution::starts_on(::beman::execution::detail::forward_like<OutSndr>(data),
                                                  ::beman::execution::detail::forward_like<OutSndr>(child)),
                    ::std::move(sch));
            }
        } else {
            auto& [sch, closure] = data;
            auto orig_sch{::beman::execution::detail::query_with_default(
                ::beman::execution::get_completion_scheduler<::beman::execution::set_value_t>,
                ::beman::execution::get_env(child),
                ::beman::execution::detail::query_with_default(
                    ::beman::execution::get_scheduler, env, not_a_scheduler{}))};

            if constexpr (::std::same_as<not_a_scheduler, decltype(orig_sch)>) {
                return env_needs_get_scheduler<Env>{};
            } else {
                return ::beman::execution::detail::write_env(
                    ::beman::execution::continues_on(
                        ::beman::execution::detail::forward_like<OutSndr>(closure)(::beman::execution::continues_on(
                            ::beman::execution::detail::write_env(
                                ::beman::execution::detail::forward_like<OutSndr>(child),
                                ::beman::execution::detail::sched_env(orig_sch)),
                            sch)),
                        orig_sch),
                    ::beman::execution::detail::sched_env(env));
            }
        }
    }

    template <::beman::execution::scheduler Sch, ::beman::execution::sender Sndr>
        requires ::beman::execution::detail::is_sender_adaptor_closure<Sndr>
    auto operator()(Sch&&, Sndr&&) const -> void =
        BEMAN_EXECUTION_DELETE("on(sch, sndr) requires that sndr isn't both a sender and sender adaptor closure");

    template <::beman::execution::scheduler                         Sch,
              ::beman::execution::sender                            Sndr,
              ::beman::execution::detail::is_sender_adaptor_closure Closure>
        requires ::beman::execution::detail::is_sender_adaptor_closure<Sndr>
    auto operator()(Sndr&&, Sch&&, Closure&&) const -> void =
        BEMAN_EXECUTION_DELETE("on(sch, sndr) requires that sndr isn't both a sender and sender adaptor closure");

    template <::beman::execution::scheduler Sch, ::beman::execution::sender Sndr>
    auto operator()(Sch&& sch, Sndr&& sndr) const {
        auto domain{::beman::execution::detail::query_with_default(
            ::beman::execution::get_domain, sch, ::beman::execution::default_domain{})};
        return ::beman::execution::transform_sender(
            domain,
            ::beman::execution::detail::make_sender(*this, ::std::forward<Sch>(sch), ::std::forward<Sndr>(sndr)));
    }
    template <::beman::execution::scheduler                         Sch,
              ::beman::execution::sender                            Sndr,
              ::beman::execution::detail::is_sender_adaptor_closure Closure>
    auto operator()(Sndr&& sndr, Sch&& sch, Closure&& closure) const {
        auto domain{::beman::execution::detail::get_domain_early(sndr)};
        return ::beman::execution::transform_sender(
            domain,
            ::beman::execution::detail::make_sender(
                *this,
                ::beman::execution::detail::product_type{::std::forward<Sch>(sch), ::std::forward<Closure>(closure)},
                ::std::forward<Sndr>(sndr)));
    }
    template <::beman::execution::scheduler Sch, ::beman::execution::detail::is_sender_adaptor_closure Closure>
    auto operator()(Sch&& sch, Closure&& closure) const {
        return ::beman::execution::detail::sender_adaptor{
            *this, ::std::forward<Sch>(sch), ::std::forward<Closure>(closure)};
    }
};

} // namespace beman::execution::detail

namespace beman::execution {
using on_t = ::beman::execution::detail::on_t;
inline constexpr ::beman::execution::on_t on{};
} // namespace beman::execution

// ----------------------------------------------------------------------------

#include <beman/execution/detail/suppress_pop.hpp>

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_ON
