// include/beman/execution/detail/is_completion_query.hpp                    -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_IS_COMPLETION_QUERY
#define INCLUDED_BEMAN_EXECUTION_DETAIL_IS_COMPLETION_QUERY

#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <type_traits>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail.get_completion_domain;
import beman.execution.detail.get_completion_scheduler;
#else
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

template <typename Query>
inline constexpr bool is_completion_query_v = ::beman::execution::detail::is_completion_query<Query>::value;
} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_IS_COMPLETION_QUERY
