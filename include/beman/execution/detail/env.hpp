// include/beman/execution/detail/env.hpp                             -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_ENV
#define INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_ENV

#include <beman/execution/detail/non_assignable.hpp>
#include <beman/execution/detail/queryable.hpp>
#include <type_traits>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
template <::beman::execution::detail::queryable>
struct env_base;

template <typename E, typename Q>
concept has_query = requires(const E& e) { e.query(::std::declval<Q>()); };

template <typename Q, typename... E>
struct find_env;
template <typename Q, typename E0, typename... E>
    requires has_query<E0, Q>
struct find_env<Q, E0, E...> {
    using type = E0;
};
template <typename Q, typename E0, typename... E>
    requires(not has_query<E0, Q>)
struct find_env<Q, E0, E...> {
    using type = typename find_env<Q, E...>::type;
};
} // namespace beman::execution::detail

namespace beman::execution {
template <::beman::execution::detail::queryable... Envs>
struct env;

template <::beman::execution::detail::queryable... Envs>
env(Envs...) -> env<::std::unwrap_reference_t<Envs>...>;
} // namespace beman::execution

// ----------------------------------------------------------------------------

template <::beman::execution::detail::queryable Env>
struct beman::execution::detail::env_base {
    Env env_;
};

template <::beman::execution::detail::queryable... Envs>
struct beman::execution::env : ::beman::execution::detail::env_base<Envs>... {
    [[no_unique_address]] ::beman::execution::detail::non_assignable na_{};

    template <typename Q>
        requires(::beman::execution::detail::has_query<Envs, Q> || ...)
    constexpr auto query(Q q) const noexcept -> decltype(auto) {
        using E = typename ::beman::execution::detail::find_env<Q, Envs...>::type;
        return q(static_cast<const ::beman::execution::detail::env_base<E>&>(*this).env_);
    }
};

// ----------------------------------------------------------------------------

#endif
