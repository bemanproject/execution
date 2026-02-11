module;
// src/beman/execution/schedule_from.cppm                             -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_HAS_MODULES
#define BEMAN_HAS_MODULES
#endif
#include <beman/execution/detail/schedule_from.hpp>

export module beman.execution.detail.schedule_from;

namespace beman::execution {
export using beman::execution::schedule_from_t;
export using beman::execution::schedule_from;
} // namespace beman::execution

namespace beman::execution::detail {
export BEMAN_SPECIALIZE_EXPORT struct impls_for<::beman::execution::schedule_from_t>;

export template <typename Scheduler, typename Sender, typename Env>
struct completion_signatures_for_impl<
    ::beman::execution::detail::basic_sender<::beman::execution::schedule_from_t, Scheduler, Sender>,
    Env>;
} // namespace beman::execution::detail
