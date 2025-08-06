// include/beman/execution/detail/scope_token.hpp                     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_SCOPE_TOKEN
#define INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_SCOPE_TOKEN

#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/sender_in.hpp>
#include <beman/execution/detail/get_completion_signatures.hpp>
#include <concepts>
#include <type_traits>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
struct token_test_env {};

struct token_test_sender {
    using sender_concept = ::beman::execution::sender_t;
    template <typename... Env>
    auto get_completion_signatures(Env const&...) const noexcept {
        return ::beman::execution::completion_signatures<>{};
    }
};
static_assert(::beman::execution::sender<::beman::execution::detail::token_test_sender>);
static_assert(::beman::execution::sender_in<::beman::execution::detail::token_test_sender,
                                            ::beman::execution::detail::token_test_env>);
} // namespace beman::execution::detail

namespace beman::execution {
template <typename Token>
concept scope_token = ::std::copyable<Token> && requires(Token token) {
    { token.try_associate() } -> ::std::same_as<bool>;
    { token.disassociate() } noexcept;
    {
        token.wrap(::std::declval<::beman::execution::detail::token_test_sender>())
    } -> ::beman::execution::sender_in<::beman::execution::detail::token_test_env>;
};
} // namespace beman::execution

// ----------------------------------------------------------------------------

#endif
