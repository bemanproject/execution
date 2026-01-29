// include/beman/execution/detail/get_env.hpp                       -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_GET_ENV
#define INCLUDED_BEMAN_EXECUTION_DETAIL_GET_ENV

#include <beman/execution/detail/common.hpp>
#include <beman/execution/detail/queryable.hpp>
#include <beman/execution/detail/env.hpp>
#include <type_traits>
#include <utility>

// ----------------------------------------------------------------------------

namespace beman::execution {
BEMAN_EXECUTION_EXPORT struct get_env_t {
    template <typename Object>
        requires(not requires(::std::add_const_t<::std::remove_cvref_t<Object>>& object) { object.get_env(); } ||
                 ::beman::execution::detail::queryable<
                     std::remove_cvref_t<decltype(::std::declval<const ::std::remove_cvref_t<Object>&>().get_env())>>)
    auto operator()(Object&& object) const noexcept -> decltype(auto) {
        ::std::add_const_t<::std::remove_cvref_t<Object>>& obj{object};
        if constexpr (requires { obj.get_env(); }) {
            static_assert(noexcept(obj.get_env()), "get_env requires the expression to be noexcept");
            static_assert(::beman::execution::detail::queryable<std::remove_cvref_t<decltype(obj.get_env())>>,
                          "get_env requires the result type to be destructible");
            return obj.get_env();
        } else {
            return ::beman::execution::env<>{};
        }
    }
};

BEMAN_EXECUTION_EXPORT inline constexpr get_env_t get_env{};
} // namespace beman::execution

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_GET_ENV
