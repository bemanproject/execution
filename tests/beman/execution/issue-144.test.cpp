// tests/beman/execution/issue-144.test.cpp                           -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/execution.hpp>

namespace bex = beman::execution;

int main() {
    double d = 19.0;
    bex::just([d](auto) { return d; });
}
