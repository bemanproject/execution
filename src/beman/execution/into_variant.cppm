module;
// src/beman/execution/into_variant.cppm                              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/detail/into_variant.hpp>

export module beman.execution.detail.into_variant;

namespace beman::execution {
export using beman::execution::into_variant;
export using beman::execution::into_variant_t;
} // namespace beman::execution

namespace beman::execution::detail {
export BEMAN_SPECIALIZE_EXPORT
struct impls_for<::beman::execution::detail::into_variant_t>;

export template <typename Sender, typename State, typename Env>
struct completion_signatures_for_impl<
    ::beman::execution::detail::basic_sender<::beman::execution::detail::into_variant_t, State, Sender>,
    Env>;
} // namespace beman::execution::detail
