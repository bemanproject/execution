module;
// src/beman/execution/when_all.cppm                                  -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/detail/when_all.hpp>

export module beman.execution.detail.when_all;

namespace beman::execution::detail {
export using beman::execution::detail::when_all_env;
} // namespace beman::execution::detail

namespace beman::execution {
export using beman::execution::when_all_t;
export using beman::execution::when_all;
} // namespace beman::execution
