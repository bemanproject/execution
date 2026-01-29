// tests/beman/execution/issue-144.test.cpp                           -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifdef BEMAN_USE_MODULE
import beman.execution;
#else
#include <beman/execution/execution.hpp>
#endif
#include <test/execution.hpp>

namespace bex = beman::execution;

TEST(issue144) {
    double d = 19.0;
    bex::just([d](auto) { return d; });
}
