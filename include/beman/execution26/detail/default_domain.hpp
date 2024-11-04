// include/beman/execution26/detail/default_domain.hpp                -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_DEFAULT_DOMAIN
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_DEFAULT_DOMAIN

#include <beman/execution26/detail/queryable.hpp>
#include <beman/execution26/detail/sender.hpp>
#include <beman/execution26/detail/sender_decompose.hpp>
#include <beman/execution26/detail/tag_of_t.hpp>
#include <type_traits>
#include <utility>

// ----------------------------------------------------------------------------

namespace beman::execution26 {
/*!
 * \brief Domain type used when no domain is specified explicitly.
 * \headerfile beman/execution26/execution.hpp <beman/execution26/execution.hpp>
 */
struct default_domain {
    template <::beman::execution26::sender Sender, ::beman::execution26::detail::queryable... Env>
        requires(sizeof...(Env) <= 1) && requires(Sender&& sender, Env&&... env) {
            ::beman::execution26::tag_of_t<Sender>().transform_sender(::std::forward<Sender>(sender),
                                                                      ::std::forward<Env>(env)...);
        }
    static constexpr auto transform_sender(Sender&& sender, Env&&... env) noexcept(
        noexcept(::beman::execution26::tag_of_t<Sender>().transform_sender(
            ::std::forward<Sender>(sender), ::std::forward<Env>(env)...))) -> ::beman::execution26::sender
        decltype(auto) {
        return ::beman::execution26::tag_of_t<Sender>().transform_sender(::std::forward<Sender>(sender),
                                                                         ::std::forward<Env>(env)...);
    }

    template <::beman::execution26::sender Sender, ::beman::execution26::detail::queryable... Env>
        requires(sizeof...(Env) <= 1) && (not requires(Sender&& sender, Env&&... env) {
                    ::beman::execution26::tag_of_t<Sender>().transform_sender(::std::forward<Sender>(sender),
                                                                              ::std::forward<Env>(env)...);
                })
    static constexpr auto transform_sender(Sender&& sender, Env&&...) noexcept(
        noexcept(::std::forward<Sender>(sender))) -> ::beman::execution26::sender decltype(auto) {
        return ::std::forward<Sender>(sender);
    }

    template <::beman::execution26::sender Sender, ::beman::execution26::detail::queryable Env>
        requires requires(Sender&& sender, Env&& env) {
            ::beman::execution26::tag_of_t<Sender>().transform_env(::std::forward<Sender>(sender),
                                                                   ::std::forward<Env>(env));
        }
    static constexpr auto transform_env(Sender&& sender, Env&& env) noexcept -> ::beman::execution26::detail::queryable
        decltype(auto) {
        return ::beman::execution26::tag_of_t<Sender>().transform_env(::std::forward<Sender>(sender),
                                                                      ::std::forward<Env>(env));
    }

    template <::beman::execution26::sender Sender, ::beman::execution26::detail::queryable Env>
        requires(not requires(Sender&& sender, Env&& env) {
                    ::beman::execution26::tag_of_t<Sender>().transform_env(::std::forward<Sender>(sender),
                                                                           ::std::forward<Env>(env));
                })
    static constexpr auto transform_env(Sender&&, Env&& env) noexcept -> ::beman::execution26::detail::queryable
        decltype(auto) {
        return static_cast<Env>(::std::forward<Env>(env));
    }

    template <typename Tag, ::beman::execution26::sender Sender, typename... Args>
        requires requires(Sender&& sender, Args&&... args) {
            Tag().apply_sender(::std::forward<Sender>(sender), ::std::forward<Args>(args)...);
        }
    static constexpr auto apply_sender(Tag, Sender&& sender, Args&&... args) noexcept(noexcept(
        Tag().apply_sender(::std::forward<Sender>(sender), ::std::forward<Args>(args)...))) -> decltype(auto) {
        return Tag().apply_sender(::std::forward<Sender>(sender), ::std::forward<Args>(args)...);
    }
};
} // namespace beman::execution26

// ----------------------------------------------------------------------------

#endif