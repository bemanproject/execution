module;
// src/beman/execution/indeterminate_domain.cppm                      -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define BEMAN_EXECUTION_DETAIL_INDETERMINATE_DOMAIN_CPPM
#include <beman/execution/detail/indeterminate_domain.hpp>

export module beman.execution.detail.indeterminate_domain;

namespace beman::execution {
export using beman::execution::indeterminate_domain;
} // namespace beman::execution

template <typename... Domains1, typename... Domains2>
struct std::common_type<::beman::execution::indeterminate_domain<Domains1...>,
                        ::beman::execution::indeterminate_domain<Domains2...>> {
    using type =
        ::beman::execution::detail::meta::unique<::beman::execution::indeterminate_domain<Domains1..., Domains2...>>;
};

template <typename... Domains, typename D>
struct std::common_type<::beman::execution::indeterminate_domain<Domains...>, D> {
    // ReSharper disable once CppFunctionIsNotImplemented
    static auto get_common_domain() noexcept -> D
        requires(sizeof...(Domains) == 0uz);

    // ReSharper disable once CppFunctionIsNotImplemented
    static auto get_common_domain() noexcept
        -> ::beman::execution::detail::meta::unique<::beman::execution::indeterminate_domain<Domains..., D>>;

    using type = decltype(get_common_domain());
};

template <typename D, typename... Domains>
struct std::common_type<D, ::beman::execution::indeterminate_domain<Domains...>>
    : ::std::common_type<::beman::execution::indeterminate_domain<Domains...>, D> {};
