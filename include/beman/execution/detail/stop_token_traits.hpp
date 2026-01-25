// include/beman/execution/detail/stop_token_traits.hpp               -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_STOP_TOKEN_TRAITS
#define INCLUDED_BEMAN_EXECUTION_DETAIL_STOP_TOKEN_TRAITS

#include <beman/execution/detail/check_type_alias_exist.hpp>
#include <stop_token>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
template <typename>
struct stoppable_token_traits;

template <typename Token>
    requires requires { typename check_type_alias_exist<Token::template callback_type>; }
struct stoppable_token_traits<Token> {
    template <typename Fn>
    using callback_type = typename Token::template callback_type<Fn>;
};

template <>
struct stoppable_token_traits<std::stop_token> {
    template <typename Fn>
    using callback_type = std::stop_callback<Fn>;
};
} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif
