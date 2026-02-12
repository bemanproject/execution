module;
// src/beman/execution/read_env.cppm                                  -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_HAS_MODULES
#define BEMAN_HAS_MODULES
#endif
#include <beman/execution/detail/read_env.hpp>

export module beman.execution.detail.read_env;

namespace beman::execution {
export using beman::execution::read_env_t;
export using beman::execution::read_env;
} // namespace beman::execution

namespace beman::execution::detail {
export BEMAN_SPECIALIZE_EXPORT struct impls_for<::beman::execution::detail::read_env_t>;
} // namespace beman::execution::detail
