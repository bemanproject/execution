module;
// src/beman/execution/sender.cppm                                    -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception


#include <beman/execution/detail/sender.hpp>

export module beman.execution.detail.sender;

namespace beman::execution::detail {
export using beman::execution::detail::enable_sender;
export using beman::execution::detail::is_sender;
} // namespace beman::execution::detail
namespace beman::execution {
export using beman::execution::sender_t;
export using beman::execution::sender;
} // namespace beman::execution
