// include/beman/execution/detail/fwd_env.hpp                       -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_FWD_ENV
#define INCLUDED_BEMAN_EXECUTION_DETAIL_FWD_ENV

#include <beman/execution/detail/common.hpp>
#include <beman/execution/detail/forwarding_query.hpp>
#include <type_traits>
#include <utility>

#include <beman/execution/detail/suppress_push.hpp>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
/*!
 * \brief A helper class to create environments and taking forwarding_query into account
 * \headerfile beman/execution/execution.hpp <beman/execution/execution.hpp>
 * \internal
 */
template <typename Env> //-dk:TODO detail export
class fwd_env {
  private:
    Env env;

  public:
    explicit fwd_env(Env&& e) : env(::std::forward<Env>(e)) {}

    template <typename Query, typename... Args>
        requires(!::beman::execution::forwarding_query(::std::remove_cvref_t<Query>()))
    constexpr auto query(Query&& q,
                         Args&&... args) const = BEMAN_EXECUTION_DELETE("the used query is not forwardable");

    template <typename Query, typename... Args>
        requires(::beman::execution::forwarding_query(::std::remove_cvref_t<Query>())) &&
                requires(const Env& e, Query&& q, Args&&... args) { e.query(q, ::std::forward<Args>(args)...); }
    constexpr auto query(Query&& q, Args&&... args) const
        noexcept(noexcept(env.query(q, ::std::forward<Args>(args)...))) {
        return env.query(q, ::std::forward<Args>(args)...);
    }
};
template <typename Env>
fwd_env(Env&&) -> fwd_env<Env>;
} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#include <beman/execution/detail/suppress_pop.hpp>

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_FWD_ENV
