// tests/beman/execution/issue-313.test.cpp                           -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <test/execution.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <coroutine>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution.hpp>
#endif

namespace ex = beman::execution;

ex::task<void> func() {
    ex::simple_counting_scope scope;
    co_await scope.join();
}

int main() { ex::sync_wait(func()); }
