// tests/beman/execution/issue-200.test.cpp                           -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <test/execution.hpp>
#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <stop_token>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution.hpp>
#endif

// ----------------------------------------------------------------------------

auto main() -> int {
#if not defined(__clang__) || __clang_major__ > 19
    static_assert(test_std::stoppable_token<std::stop_token>);
#endif
    return 0;
}
