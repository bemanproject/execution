// include/beman/execution/detail/hide_completion_queries.hpp                      -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_HIDE_COMPLETION_QUERIES
#define INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_HIDE_COMPLETION_QUERIES

#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <type_traits>
#include <utility>
#endif
#if BEMAN_HAS_MODULES
import beman.execution.detail.fwd_env;
import beman.execution.detail.get_completion_domain;
import beman.execution.detail.get_completion_scheduler;
#else
#include <beman/execution/detail/fwd_env.hpp>
#include <beman/execution/detail/get_completion_domain.hpp>
#include <beman/execution/detail/get_completion_scheduler.hpp>
#endif

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
template <typename>
struct is_completion_query : ::std::false_type {};

template <typename Tag>
struct is_completion_query<::beman::execution::get_completion_scheduler_t<Tag>> : ::std::true_type {};

template <typename Tag>
struct is_completion_query<::beman::execution::get_completion_domain_t<Tag>> : ::std::true_type {};

template <typename ChildAttrs>
struct hide_completion_queries {
    template <typename Query, typename... Args>
        requires(!::beman::execution::detail::is_completion_query<Query>::value) &&
                requires(const Query& q, const ChildAttrs& child_attrs, Args&&... args) {
                    ::beman::execution::detail::fwd_env(child_attrs).query(q, ::std::forward<Args>(args)...);
                }
    auto query(Query q, Args&&... args) const noexcept {
        return ::beman::execution::detail::fwd_env(child_attrs).query(q, ::std::forward<Args>(args)...);
    }

    ::std::remove_cvref_t<ChildAttrs> child_attrs;
};
} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif // INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_HIDE_COMPLETION_QUERIES
