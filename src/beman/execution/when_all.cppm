module;
// src/beman/execution/when_all.cppm                                  -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/detail/when_all.hpp>

export module beman.execution.detail.when_all;

namespace beman::execution {
export using beman::execution::when_all_t;
export using beman::execution::when_all;
} // namespace beman::execution

namespace beman::execution::detail {
export BEMAN_SPECIALIZE_EXPORT struct impls_for<::beman::execution::detail::when_all_t>;
export template <typename Data, typename Env, typename... Sender>
struct completion_signatures_for_impl<
    ::beman::execution::detail::basic_sender<::beman::execution::detail::when_all_t, Data, Sender...>,
    Env>;
} // namespace beman::execution::detail
