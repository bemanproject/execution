// include/beman/execution/detail/transform_sender.hpp              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_TRANSFORM_SENDER
#define INCLUDED_BEMAN_EXECUTION_DETAIL_TRANSFORM_SENDER

#include <beman/execution/detail/common.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/default_domain.hpp>
#include <concepts>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
template <typename Domain, ::beman::execution::sender Sender, typename... Env>
    requires(sizeof...(Env) < 2) &&
            requires(Domain dom, Sender&& sender, const Env&... env) {
                dom.transform_sender(::std::forward<Sender>(sender), env...);
            } &&
            (::std::same_as<::std::remove_cvref_t<Sender>,
                            std::remove_cvref_t<decltype(::std::declval<Domain>().transform_sender(
                                ::std::declval<Sender>(), ::std::declval<const Env&>()...))>>)
constexpr auto transform_sender(Domain, Sender&& sender, const Env&...) noexcept -> ::beman::execution::sender auto {
    return ::std::forward<Sender>(sender);
}

template <typename Domain, ::beman::execution::sender Sender, typename... Env>
    requires(sizeof...(Env) < 2) &&
            requires(Domain dom, Sender&& sender, const Env&... env) {
                dom.transform_sender(::std::forward<Sender>(sender), env...);
            } &&
            (!::std::same_as<::std::remove_cvref_t<Sender>,
                             std::remove_cvref_t<decltype(::std::declval<Domain>().transform_sender(
                                 ::std::declval<Sender>(), ::std::declval<const Env&>()...))>>)
constexpr auto transform_sender(Domain dom, Sender&& sender, const Env&... env) noexcept -> ::beman::execution::sender
    decltype(auto) {
    return ::beman::execution::detail::transform_sender(
        dom, dom.transform_sender(::std::forward<Sender>(sender), env...), env...);
}

template <typename Domain, ::beman::execution::sender Sender, typename... Env>
    requires(not requires(Domain dom, Sender&& sender, const Env&... env) {
                dom.transform_sender(::std::forward<Sender>(sender), env...);
            }) && ::std::same_as<::std::remove_cvref_t<Sender>,
                                 ::std::remove_cvref_t<decltype(::beman::execution::default_domain{}.transform_sender(
                                     ::std::declval<Sender>(), ::std::declval<Env>()...))>>
constexpr auto
    transform_sender(Domain, Sender&& sender, const Env&...) noexcept(noexcept(::std::forward<Sender>(sender)))
        -> ::beman::execution::sender auto {
    return sender;
}

template <typename Domain, ::beman::execution::sender Sender, typename... Env>
    requires(not requires(Domain dom, Sender&& sender, const Env&... env) {
                dom.transform_sender(::std::forward<Sender>(sender), env...);
            }) &&
            (!::std::same_as<::std::remove_cvref_t<Sender>,
                             ::std::remove_cvref_t<decltype(::beman::execution::default_domain{}.transform_sender(
                                 ::std::declval<Sender>(), ::std::declval<Env>()...))>>)
constexpr auto transform_sender(Domain dom, Sender&& sender, const Env&... env) noexcept(noexcept(
    ::beman::execution::default_domain{}.transform_sender(::std::declval<Sender>(), ::std::declval<Env>()...)))
    -> ::beman::execution::sender decltype(auto) {
    return ::beman::execution::detail::transform_sender(
        dom, ::beman::execution::default_domain{}.transform_sender(::std::forward<Sender>(sender), env...), env...);
}
} // namespace beman::execution::detail

namespace beman::execution {
BEMAN_EXECUTION_EXPORT template <typename Domain, ::beman::execution::sender Sender, typename... Env>
    requires(sizeof...(Env) < 2) &&
            requires(Domain dom, Sender&& sender, const Env&... env) {
                dom.transform_sender(::std::forward<Sender>(sender), env...);
            } &&
            (::std::same_as<::std::remove_cvref_t<Sender>,
                            std::remove_cvref_t<decltype(::std::declval<Domain>().transform_sender(
                                ::std::declval<Sender>(), ::std::declval<const Env&>()...))>>)
constexpr auto transform_sender(Domain, Sender&& sender, const Env&...) noexcept -> ::beman::execution::sender
    decltype(auto) {
    return ::std::forward<Sender>(sender);
}

BEMAN_EXECUTION_EXPORT template <typename Domain, ::beman::execution::sender Sender, typename... Env>
    requires(sizeof...(Env) < 2) &&
            requires(Domain dom, Sender&& sender, const Env&... env) {
                dom.transform_sender(::std::forward<Sender>(sender), env...);
            } &&
            (!::std::same_as<::std::remove_cvref_t<Sender>,
                             std::remove_cvref_t<decltype(::std::declval<Domain>().transform_sender(
                                 ::std::declval<Sender>(), ::std::declval<const Env&>()...))>>)
constexpr auto transform_sender(Domain dom, Sender&& sender, const Env&... env) noexcept -> ::beman::execution::sender
    auto {
    return ::beman::execution::detail::transform_sender(
        dom, dom.transform_sender(::std::forward<Sender>(sender), env...), env...);
}

BEMAN_EXECUTION_EXPORT template <typename Domain, ::beman::execution::sender Sender, typename... Env>
    requires(sizeof...(Env) < 2) && (not requires(Domain dom, Sender&& sender, const Env&... env) {
                dom.transform_sender(::std::forward<Sender>(sender), env...);
            }) &&
            ::std::same_as<::std::remove_cvref_t<Sender>,
                           ::std::remove_cvref_t<decltype(::beman::execution::default_domain{}.transform_sender(
                               ::std::declval<Sender>(), ::std::declval<Env>()...))>>
constexpr auto
    transform_sender(Domain, Sender&& sender, const Env&...) noexcept(noexcept(::std::forward<Sender>(sender)))
        -> ::beman::execution::sender decltype(auto) {
    return ::std::forward<Sender>(sender);
}

BEMAN_EXECUTION_EXPORT template <typename Domain, ::beman::execution::sender Sender, typename... Env>
    requires(sizeof...(Env) < 2) && (not requires(Domain dom, Sender&& sender, const Env&... env) {
                dom.transform_sender(::std::forward<Sender>(sender), env...);
            }) &&
            (!::std::same_as<::std::remove_cvref_t<Sender>,
                             ::std::remove_cvref_t<decltype(::beman::execution::default_domain{}.transform_sender(
                                 ::std::declval<Sender>(), ::std::declval<Env>()...))>>)
constexpr auto transform_sender(Domain dom, Sender&& sender, const Env&... env) noexcept(
    noexcept(::beman::execution::detail::transform_sender(
        dom,
        ::beman::execution::default_domain{}.transform_sender(::std::declval<Sender>(), ::std::declval<Env>()...),
        env...))) -> ::beman::execution::sender decltype(auto) {
    return ::beman::execution::detail::transform_sender(
        dom, ::beman::execution::default_domain{}.transform_sender(::std::forward<Sender>(sender), env...), env...);
}
} // namespace beman::execution

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_TRANSFORM_SENDER
