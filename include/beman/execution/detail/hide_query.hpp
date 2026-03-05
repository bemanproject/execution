// include/beman/execution/detail/hide_query.hpp                      -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_HIDE_QUERY
#define INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_HIDE_QUERY

#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <concepts>
#endif
#if BEMAN_HAS_MODULES
import beman.execution.detail.queryable;
import beman.execution.detail.get_domain;
import beman.execution.detail.get_scheduler;
#else
#include <beman/execution/detail/queryable.hpp>
#include <beman/execution/detail/get_domain.hpp>
#include <beman/execution/detail/get_scheduler.hpp>
#endif

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
template <::beman::execution::detail::queryable Q>
struct hide_query_t {
    template <typename Tag, typename... Args>
    auto query(Tag&& tag, Args&&... args) const noexcept -> decltype(auto) {
        return q.query(::std::forward<Tag>(tag), ::std::forward<Args>(args)...);
    }
    template <typename... Args>
    auto query(::beman::execution::get_domain_t, Args&&... args) const noexcept -> void = delete;
    template <typename... Args>
    auto query(::beman::execution::get_scheduler_t, Args&&... args) const noexcept -> void = delete;

    const Q& q;
};

template <::beman::execution::detail::queryable Q>
auto hide_query(const Q& q) noexcept {
    return ::beman::execution::detail::hide_query_t<Q>{q};
}
} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif
