// src/beman/execution/tests/thread-stoptoken.test.cpp
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <test/execution.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution/stop_token.hpp>
#endif

TEST(thread_stoptoken) {
    // section [thread.stoptoken] is empty
}
