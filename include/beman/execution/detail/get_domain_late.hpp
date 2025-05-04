// include/beman/execution/detail/get_domain_late.hpp               -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_GET_DOMAIN_LATE
#define INCLUDED_BEMAN_EXECUTION_DETAIL_GET_DOMAIN_LATE

#include <beman/execution/detail/default_domain.hpp>
#include <beman/execution/detail/completion_domain.hpp>
#include <beman/execution/detail/get_env.hpp>
#include <beman/execution/detail/get_domain.hpp>
#include <beman/execution/detail/get_scheduler.hpp>
#include <beman/execution/detail/sender_decompose.hpp>
#include <beman/execution/detail/tag_of_t.hpp>
#include <concepts>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
template <typename T>
concept not_void = !::std::same_as<T, void>;

template <typename Tag>
struct get_domain_late_helper {
    template <typename Sender, typename Env>
    static constexpr auto get(const Sender& sender, const Env& env) noexcept {
        if constexpr (requires {
                          {
                              ::beman::execution::get_domain(::beman::execution::get_env(sender))
                          } -> ::beman::execution::detail::not_void;
                      })
            return ::beman::execution::get_domain(::beman::execution::get_env(sender));
        else if constexpr (requires {
                               {
                                   ::beman::execution::detail::completion_domain<void>(sender)
                               } -> ::beman::execution::detail::not_void;
                           })
            return ::beman::execution::detail::completion_domain<void>(sender);
        else if constexpr (requires {
                               { ::beman::execution::get_domain(env) } -> ::beman::execution::detail::not_void;
                           })
            return ::beman::execution::get_domain(env);
        else if constexpr (requires {
                               {
                                   ::beman::execution::get_domain(::beman::execution::get_scheduler(env))
                               } -> ::beman::execution::detail::not_void;
                           })
            return ::beman::execution::get_domain(::beman::execution::get_scheduler(env));
        else
            return ::beman::execution::default_domain();
    }
};
template <typename Sender, typename Env>
constexpr auto get_domain_late(const Sender& sender, const Env& env) noexcept {
    using tag_t = ::beman::execution::tag_of_t<Sender>;
    return ::beman::execution::detail::get_domain_late_helper<tag_t>::get(sender, env);
}
} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif
