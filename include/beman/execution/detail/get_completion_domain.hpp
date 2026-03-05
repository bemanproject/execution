// include/beman/execution/detail/get_completion_domain.hpp           -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_GET_COMPLETION_DOMAIN
#define INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_GET_COMPLETION_DOMAIN

#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <concepts>
#endif
#if BEMAN_HAS_MODULES
import beman.execution.detail.set_error;
import beman.execution.detail.set_stopped;
import beman.execution.detail.set_value;
#else
#include <beman/execution/detail/set_error.hpp>
#include <beman/execution/detail/set_stopped.hpp>
#include <beman/execution/detail/set_value.hpp>
#endif

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
template <typename T = void>
    requires(::std::same_as<T, void> || ::std::same_as<T, ::beman::execution::set_error_t> ||
             ::std::same_as<T, ::beman::execution::set_stopped_t> ||
             ::std::same_as<T, ::beman::execution::set_value_t>)
struct get_completion_domain_t {};
} // namespace beman::execution::detail

namespace beman::execution {
template <typename CPO = void>
using get_completion_domain_t = detail::get_completion_domain_t<CPO>;
template <typename CPO = void>
inline constexpr get_completion_domain_t<CPO> get_completion_domain{};
} // namespace beman::execution

// ----------------------------------------------------------------------------

#endif
