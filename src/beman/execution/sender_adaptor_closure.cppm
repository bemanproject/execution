module;
// src/beman/execution/sender_adaptor_closure.cppm                    -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/detail/sender_adaptor_closure.hpp>

export module beman.execution.detail.sender_adaptor_closure;

namespace beman::execution {
export using beman::execution::detail::pipeable::operator|;
}
namespace beman::execution::detail {
export using beman::execution::detail::is_sender_adaptor_closure;
}
namespace beman::execution {
export using beman::execution::sender_adaptor_closure;
} // namespace beman::execution
