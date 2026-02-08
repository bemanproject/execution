module;
// src/beman/execution/get_stop_token.cppm                            -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_HAS_MODULES
#define BEMAN_HAS_MODULES
#endif
#include <beman/execution/detail/get_stop_token.hpp>

export module beman.execution.detail.get_stop_token;

namespace beman::execution {
export using beman::execution::get_stop_token_t;
export using beman::execution::get_stop_token;
} // namespace beman::execution
