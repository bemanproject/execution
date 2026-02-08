module;
// src/beman/execution/split.cppm                                     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_HAS_MODULES
#define BEMAN_HAS_MODULES
#endif
#include <beman/execution/detail/split.hpp>

export module beman.execution.detail.split;

namespace beman::execution {
export using beman::execution::split_t;
export using beman::execution::split;
} // namespace beman::execution

namespace beman::execution::detail {
export BEMAN_SPECIALIZE_EXPORT struct impls_for<split_impl_t>;
export template <class Sndr, class Env>
struct completion_signatures_for_impl<
    ::beman::execution::detail::basic_sender<::beman::execution::detail::split_impl_t,
                                             ::beman::execution::detail::shared_wrapper<Sndr>>,
    Env>;
} // namespace beman::execution::detail
