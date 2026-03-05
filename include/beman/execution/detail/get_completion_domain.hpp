// include/beman/execution/detail/get_completion_domain.hpp           -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_GET_COMPLETION_DOMAIN
#define INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_GET_COMPLETION_DOMAIN

#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <concepts>
#include <type_traits>
#include <utility>
#endif
#if BEMAN_HAS_MODULES
import beman.execution.detail.default_domain;
import beman.execution.detail.get_completion_scheduler;
import beman.execution.detail.queryable;
import beman.execution.detail.scheduler;
import beman.execution.detail.set_error;
import beman.execution.detail.set_stopped;
import beman.execution.detail.set_value;
import beman.execution.detail.try_query;
#else
#include <beman/execution/detail/default_domain.hpp>
#include <beman/execution/detail/get_completion_scheduler.hpp>
#include <beman/execution/detail/queryable.hpp>
#include <beman/execution/detail/scheduler.hpp>
#include <beman/execution/detail/set_error.hpp>
#include <beman/execution/detail/set_stopped.hpp>
#include <beman/execution/detail/set_value.hpp>
#include <beman/execution/detail/try_query.hpp>
#endif

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
template <typename T = void>
    requires(::std::same_as<T, void> || ::std::same_as<T, ::beman::execution::set_error_t> ||
             ::std::same_as<T, ::beman::execution::set_stopped_t> ||
             ::std::same_as<T, ::beman::execution::set_value_t>)
struct get_completion_domain_t {
    template <typename Q, typename... E>
    auto operator()(Q&& q, E&&... e) const {
        using domain = decltype([&]{
            if constexpr (requires{ ::beman::execution::detail::try_query(::std::forward<Q>(q), get_completion_domain_t<T>{}, ::std::forward<E>(e)...); }) {
                return ::beman::execution::detail::try_query(::std::forward<Q>(q), get_completion_domain_t<T>{}, ::std::forward<E>(e)...);
            } else if constexpr (::std::same_as<T, void>) {
                return get_completion_domain_t<::beman::execution::set_value_t>()(::std::forward<Q>(q), ::std::forward<E>(e)...);
            }
            else if constexpr (::beman::execution::detail::try_query(::beman::execution::get_completion_scheduler<T>(q, e...), get_completion_domain_t<::beman::execution::set_value_t>{}, ::std::forward<Q>(q), ::std::forward<E>(e)...)) {
                return ::beman::execution::detail::try_query(::beman::execution::get_completion_scheduler<T>(q, e...), get_completion_domain_t<::beman::execution::set_value_t>{}, ::std::forward<Q>(q), ::std::forward<E>(e)...);
            } else if constexpr (::beman::execution::scheduler<Q> && 0u != sizeof...(E)) {
                return ::beman::execution::default_domain{};
            }
            else {
                static_assert(::std::same_as<T, int>, "get_completion_domain requires the environment to be queryable for get_completion_domain or get_completion_scheduler");
            }
        }());
        static_assert(noexcept(domain{}), "the domain's default constructor has to be noexcept");
        return domain{};
    }
};
} // namespace beman::execution::detail

namespace beman::execution {
template <typename CPO = void>
using get_completion_domain_t = detail::get_completion_domain_t<CPO>;
template <typename CPO = void>
inline constexpr get_completion_domain_t<CPO> get_completion_domain{};
} // namespace beman::execution

// ----------------------------------------------------------------------------

#endif
