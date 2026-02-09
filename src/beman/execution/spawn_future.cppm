module;
// src/beman/execution/spawn_future.cppm                              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_HAS_MODULES
#define BEMAN_HAS_MODULES
#endif
#if !defined(__GNUC__) || defined(__clang__)
#include <beman/execution/detail/spawn_future.hpp>
#endif

export module beman.execution.detail.spawn_future;

#if !defined(__GNUC__) || defined(__clang__)
namespace beman::execution {
export using beman::execution::spawn_future_t;
export using beman::execution::spawn_future;
} // namespace beman::execution

namespace beman::execution::detail {
export using beman::execution::detail::spawn_future_receiver;
export using beman::execution::detail::spawn_future_state_base;

export BEMAN_SPECIALIZE_EXPORT struct impls_for<spawn_future_t>;
export template <typename State, typename Deleter, typename Env>
struct completion_signatures_for_impl<
    ::beman::execution::detail::basic_sender<::beman::execution::detail::spawn_future_t,
                                             ::std::unique_ptr<State, Deleter>>,
    Env>;
} // namespace beman::execution::detail
#endif
