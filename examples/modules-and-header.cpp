// examples/modules-and-header.cpp                                    -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <tuple>
import beman.execution;
namespace ex = beman::execution;

int main() {
    auto [rc] = *ex::sync_wait(ex::just(0));
    return rc;
}
