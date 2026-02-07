// examples/just_stopped.cpp                                          -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution/execution.hpp>
#endif
namespace ex = beman::execution;

struct receiver {
    using receiver_concept = ex::receiver_t;
    auto set_value(auto&&...) noexcept -> void {}
    auto set_error(auto&&) noexcept -> void {}
    auto set_stopped() noexcept -> void {}
};

int main() {
    // ex::sync_wait(ex::just_stopped() | ex::then([]{}));
    auto then = ex::just_stopped() | ex::then([] {});
    static_assert(std::same_as<void, decltype(ex::get_completion_signatures(then, ex::env<>()))>);

    ex::connect(ex::just_stopped() | ex::then([] {}), receiver{});
}
