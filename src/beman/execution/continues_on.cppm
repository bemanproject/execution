module;
// src/beman/execution/continues_on.cppm                              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <iostream>
#ifndef BEMAN_HAS_MODULES
#define BEMAN_HAS_MODULES
#endif
#include <beman/execution/detail/continues_on.hpp>

export module beman.execution.detail.continues_on;

namespace beman::execution {
export using beman::execution::continues_on_t;
export using beman::execution::continues_on;
} // namespace beman::execution

namespace beman::execution::detail {
export BEMAN_SPECIALIZE_EXPORT struct impls_for<::beman::execution::detail::continues_on_t>;

export template <typename Env, typename...T>
auto get_domain_late(::beman::execution::detail::basic_sender<::beman::execution::detail::continues_on_t, T...> const& sender, Env&&) {
    std::cout << "get_domain_late\n";
    auto scheduler{sender.template get<1>()};
    return ::beman::execution::detail::query_with_default(
        ::beman::execution::get_domain, scheduler, ::beman::execution::default_domain{});
}
export template <typename Env, typename...T>
auto get_domain_late(::beman::execution::detail::basic_sender<::beman::execution::detail::continues_on_t, T...>&& sender, Env&&) {
    std::cout << "get_domain_late\n";
    auto scheduler{sender.template get<1>()};
    return ::beman::execution::detail::query_with_default(
        ::beman::execution::get_domain, scheduler, ::beman::execution::default_domain{});
}
export template <typename Env, typename...T>
auto get_domain_late(::beman::execution::detail::basic_sender<::beman::execution::detail::continues_on_t, T...>& sender, Env&&) {
    std::cout << "get_domain_late\n";
    auto scheduler{sender.template get<1>()};
    return ::beman::execution::detail::query_with_default(
        ::beman::execution::get_domain, scheduler, ::beman::execution::default_domain{});
}
} // namespace beman::execution::detail
