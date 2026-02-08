module;
// src/beman/execution/notify.cppm                                    -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/detail/notify.hpp>

export module beman.execution.detail.notify;

namespace beman::execution::detail {
export using beman::execution::detail::notifier;
export using beman::execution::detail::notify_t;
export using beman::execution::detail::notify;

export BEMAN_SPECIALIZE_EXPORT
struct impls_for<::beman::execution::detail::notify_t>;

export template <typename Notifier, typename Env>
struct completion_signatures_for_impl<
    ::beman::execution::detail::basic_sender<::beman::execution::detail::notify_t, Notifier>,
    Env>;
} // namespace beman::execution::detail
