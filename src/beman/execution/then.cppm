module;
// src/beman/execution/then.cppm                                      -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_HAS_MODULES
#define BEMAN_HAS_MODULES
#endif
#include <beman/execution/detail/then.hpp>

export module beman.execution.detail.then;

namespace beman::execution {
export using beman::execution::then_t;
export using beman::execution::then;
export using beman::execution::upon_error_t;
export using beman::execution::upon_error;
export using beman::execution::upon_stopped_t;
export using beman::execution::upon_stopped;
} // namespace beman::execution

namespace beman::execution::detail {
export template <typename Completion, typename Fun, typename Sender, typename Env>
struct completion_signatures_for_impl<
    ::beman::execution::detail::basic_sender<::beman::execution::detail::then_t<Completion>, Fun, Sender>,
    Env>;
export template <typename Completion>
struct impls_for<then_t<Completion>>;
} // namespace beman::execution::detail
