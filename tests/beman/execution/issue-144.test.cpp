// tests/beman/execution/issue-144.test.cpp                           -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <test/execution.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution/execution.hpp>
#endif

namespace bex = beman::execution;

TEST(issue144) {
    double d = 19.0;
    bex::just([d](auto) { return d; });
}
