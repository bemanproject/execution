// tests/beman/execution/issue-200.test.cpp                           -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/detail/stoppable_token.hpp>
#include <test/execution.hpp>
#include <stop_token>

// ----------------------------------------------------------------------------

auto main() -> int {
    static_assert(test_std::stoppable_token<std::stop_token>);
    return 0;
}