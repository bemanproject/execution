module;
// src/beman/execution/just.cppm                                      -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/detail/just.hpp>

export module beman.execution.detail.just;

namespace beman::execution {
export using beman::execution::just_t;
export using beman::execution::just;
export using beman::execution::just_error_t;
export using beman::execution::just_error;
export using beman::execution::just_stopped_t;
export using beman::execution::just_stopped;
} // namespace beman::execution

namespace beman::execution::detail {
export template <typename Completion, typename... T, typename Env>
struct completion_signatures_for_impl<
    ::beman::execution::detail::basic_sender<just_t<Completion>, ::beman::execution::detail::product_type<T...>>,
    Env>;
export template <typename Completion>
struct impls_for<just_t<Completion>>;
} // namespace beman::execution::detail
