module;
// src/beman/execution/continues_on.cppm                              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/detail/continues_on.hpp>

export module beman.execution.detail.continues_on;

namespace beman::execution {
export using beman::execution::continues_on_t;
export using beman::execution::continues_on;
} // namespace beman::execution

namespace beman::execution::detail {
export BEMAN_SPECIALIZE_EXPORT struct impls_for<::beman::execution::detail::continues_on_t>;
} // namespace beman::execution::detail
