module;
// src/beman/execution/sender_adaptor_closure.cppm                    -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/detail/sender_adaptor_closure.hpp>

export module beman.execution.detail.sender_adaptor_closure;

namespace beman::execution {
export using ::beman::execution::operator|;
}
namespace beman::execution::detail {
export using ::beman::execution::detail::is_sender_adaptor_closure;
export using ::beman::execution::detail::sender_adaptor_closure_for;
export using ::beman::execution::detail::is_sender_adaptor_closure_base;
export using ::beman::execution::detail::apply_cvref_t;
export using ::beman::execution::detail::composed_sender_adaptor_closure;
export using ::beman::execution::detail::bound_sender_adaptor_closure;
export using ::beman::execution::detail::make_sender_adaptor;

} // namespace beman::execution::detail
namespace beman::execution {
export using ::beman::execution::sender_adaptor_closure;

} // namespace beman::execution
