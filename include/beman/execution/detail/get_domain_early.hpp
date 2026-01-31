// include/beman/execution/detail/get_domain_early.hpp              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_GET_DOMAIN_EARLY
#define INCLUDED_BEMAN_EXECUTION_DETAIL_GET_DOMAIN_EARLY

#include <beman/execution/detail/common.hpp>
#include <beman/execution/detail/completion_domain.hpp>
#include <beman/execution/detail/default_domain.hpp>
#include <beman/execution/detail/get_domain.hpp>
#include <beman/execution/detail/get_env.hpp>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
template <typename Sender>
constexpr auto get_domain_early(const Sender& sender) noexcept {
    if constexpr (requires { ::beman::execution::get_domain(::beman::execution::get_env(sender)); })
        return decltype(::beman::execution::get_domain(::beman::execution::get_env(sender))){};
    else if constexpr (requires { ::beman::execution::detail::completion_domain(sender); })
        return decltype(::beman::execution::detail::completion_domain(sender)){};
    else
        return ::beman::execution::default_domain{};
}
} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_GET_DOMAIN_EARLY
