module;
// src/beman/execution/counting_scope_join.cppm                       -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_HAS_MODULES
#define BEMAN_HAS_MODULES
#endif
#include <beman/execution/detail/counting_scope_join.hpp>

export module beman.execution.detail.counting_scope_join;

namespace beman::execution::detail {
export using beman::execution::detail::counting_scope_join;

export BEMAN_SPECIALIZE_EXPORT struct impls_for<::beman::execution::detail::counting_scope_join_t>;

export template <typename Env>
struct completion_signatures_for_impl<
    ::beman::execution::detail::basic_sender<::beman::execution::detail::counting_scope_join_t,
                                             ::beman::execution::detail::counting_scope_base*>,
    Env>;
} // namespace beman::execution::detail
