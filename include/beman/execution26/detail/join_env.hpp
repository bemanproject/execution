// include/beman/execution26/detail/join_env.hpp                      -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_JOIN_ENV
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_JOIN_ENV

#include <type_traits>
#include <utility>

// ----------------------------------------------------------------------------

namespace beman::execution26::detail {
template <typename Env1, typename Env2>
class join_env {
  private:
    Env1 env1;
    Env2 env2;

  public:
    template <typename E1, typename E2>
    join_env(E1&& env1, E2&& env2) : env1(::std::forward<E1>(env1)), env2(::std::forward<E2>(env2)) {}

    template <typename Query, typename... Args>
        requires(
            requires(Env1&, const Query& query, Args&&... args) {
                env1.query(query, ::std::forward<Args>(args)...);
            } ||
            requires(Env2& env2, const Query& query, Args&&... args) {
                env2.query(query, ::std::forward<Args>(args)...);
            })
    auto query(const Query& query, Args&&... args) noexcept -> decltype(auto) {
        if constexpr (requires { env1.query(query, ::std::forward<Args>(args)...); }) {
            return env1.query(query, ::std::forward<Args>(args)...);
        } else {
            return env2.query(query, ::std::forward<Args>(args)...);
        }
    }
    template <typename Query, typename... Args>
        requires(
            requires(const Env1&, const Query& query, Args&&... args) {
                env1.query(query, ::std::forward<Args>(args)...);
            } ||
            requires(const Env2& env2, const Query& query, Args&&... args) {
                env2.query(query, ::std::forward<Args>(args)...);
            })
    auto query(const Query& query, Args&&... args) const noexcept -> decltype(auto) {
        if constexpr (requires { env1.query(query, ::std::forward<Args>(args)...); }) {
            return env1.query(query, ::std::forward<Args>(args)...);
        } else {
            return env2.query(query, ::std::forward<Args>(args)...);
        }
    }
};

template <typename Env1, typename Env2>
join_env(Env1&&, Env2&&) -> join_env<::std::remove_cvref_t<Env1>, ::std::remove_cvref_t<Env2>>;
} // namespace beman::execution26::detail

// ----------------------------------------------------------------------------

#endif