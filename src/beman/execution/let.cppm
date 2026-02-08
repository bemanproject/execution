module;
// src/beman/execution/let.cppm                                       -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/detail/let.hpp>

export module beman.execution.detail.let;

namespace beman::execution {
export using beman::execution::let_value_t;
export using beman::execution::let_value;
export using beman::execution::let_error_t;
export using beman::execution::let_error;
export using beman::execution::let_stopped_t;
export using beman::execution::let_stopped;
} // namespace beman::execution

namespace beman::execution::detail {
export template <typename Completion>
struct impls_for<::beman::execution::detail::let_t<Completion>>;

export template <typename Completion, typename Fun, typename Sender, typename Env>
struct completion_signatures_for_impl<
    ::beman::execution::detail::basic_sender<::beman::execution::detail::let_t<Completion>, Fun, Sender>,
    Env>;
} // namespace beman::execution::detail
