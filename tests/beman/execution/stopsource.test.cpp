// src/beman/execution/tests/stopsource.test.cpp
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <test/execution.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution/stop_token.hpp>
#endif

TEST(stopsource) {
    // section [thread] is empty
}
