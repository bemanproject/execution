module;
// src/beman/execution/sender_decompose.cppm                          -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_HAS_MODULES
#define BEMAN_HAS_MODULES
#endif
#include <beman/execution/detail/sender_decompose.hpp>

export module beman.execution.detail.sender_decompose;

namespace beman::execution::detail {
export using beman::execution::detail::get_sender_data;
export using beman::execution::detail::get_sender_meta;
} // namespace beman::execution::detail
