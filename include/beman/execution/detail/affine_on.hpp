// include/beman/execution/detail/affine_on.hpp                       -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_AFFINE_ON
#define INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_AFFINE_ON

#include <beman/execution/detail/env.hpp>
#include <beman/execution/detail/env.hpp>
#include <beman/execution/detail/forward_like.hpp>
#include <beman/execution/detail/fwd_env.hpp>
#include <beman/execution/detail/get_domain_early.hpp>
#include <beman/execution/detail/get_scheduler.hpp>
#include <beman/execution/detail/get_stop_token.hpp>
#include <beman/execution/detail/join_env.hpp>
#include <beman/execution/detail/make_sender.hpp>
#include <beman/execution/detail/never_stop_token.hpp>
#include <beman/execution/detail/prop.hpp>
#include <beman/execution/detail/schedule_from.hpp>
#include <beman/execution/detail/scheduler.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/sender_adaptor.hpp>
#include <beman/execution/detail/sender_adaptor_closure.hpp>
#include <beman/execution/detail/tag_of_t.hpp>
#include <beman/execution/detail/transform_sender.hpp>
#include <beman/execution/detail/write_env.hpp>

#include <concepts>
#include <type_traits>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
struct affine_on_t : ::beman::execution::sender_adaptor_closure<affine_on_t> {
    template <::beman::execution::sender Sender>
    auto operator()(Sender&& sender) const {
        return ::beman::execution::detail::transform_sender(
            ::beman::execution::detail::get_domain_early(sender),
            ::beman::execution::detail::make_sender(
                *this, ::beman::execution::env<>{}, ::std::forward<Sender>(sender)));
    }
    auto operator()() const { return ::beman::execution::detail::sender_adaptor{*this}; }

    template <::beman::execution::sender Sender, typename Env>
        requires requires(const Env& env) {
            { ::beman::execution::get_scheduler(env) } -> ::beman::execution::scheduler;
            { ::beman::execution::schedule(::beman::execution::get_scheduler(env)) } -> ::beman::execution::sender;
            {
                ::beman::execution::get_completion_signatures(
                    ::beman::execution::schedule(::beman::execution::get_scheduler(env)),
                    ::beman::execution::detail::join_env(
                        ::beman::execution::env{::beman::execution::prop{::beman::execution::get_stop_token,
                                                                         ::beman::execution::never_stop_token{}}},
                        env))
            } -> ::std::same_as<::beman::execution::completion_signatures<::beman::execution::set_value_t()>>;
        }
    static auto transform_sender(Sender&& sender, const Env& env) {
        auto& [tag, data, child] = sender;
        using child_tag_t        = ::beman::execution::tag_of_t<::std::remove_cvref_t<decltype(child)>>;

        if constexpr (requires(const child_tag_t& t) {
                          {
                              t.affine_on(::beman::execution::detail::forward_like<Sender>(child), env)
                          } -> ::beman::execution::sender;
                      }) {
            return child_tag_t{}.affine_on(::beman::execution::detail::forward_like<Sender>(child), env);
        } else {
            return ::beman::execution::write_env(
                ::beman::execution::schedule_from(
                    ::beman::execution::get_scheduler(env),
                    ::beman::execution::write_env(::beman::execution::detail::forward_like<Sender>(child), env)),
                ::beman::execution::detail::join_env(
                    ::beman::execution::env{::beman::execution::prop{::beman::execution::get_stop_token,
                                                                     ::beman::execution::never_stop_token{}}},
                    env));
        }
    }
};
} // namespace beman::execution::detail

namespace beman::execution {
using beman::execution::detail::affine_on_t;
inline constexpr affine_on_t affine_on{};
} // namespace beman::execution

// ----------------------------------------------------------------------------

#endif
