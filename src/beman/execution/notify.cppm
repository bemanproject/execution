module;
// src/beman/execution/notify.cppm                                    -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_HAS_MODULES
#define BEMAN_HAS_MODULES
#endif
#include <beman/execution/detail/notify.hpp>

export module beman.execution.detail.notify;

namespace beman::execution::detail {
export using beman::execution::detail::notifier;
export using beman::execution::detail::notify_t;
export using beman::execution::detail::notify;
} // namespace beman::execution::detail
