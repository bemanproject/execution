// examples/modules_and_header.cpp                                    -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <tuple>
#endif
import beman.execution;
namespace ex = beman::execution;

int main() {
    auto [rc] = *ex::sync_wait(ex::just(0));
    return rc;
}
