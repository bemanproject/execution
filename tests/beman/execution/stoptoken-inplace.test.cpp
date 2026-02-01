// src/beman/execution/tests/stoptoken-inplace.test.cpp
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <test/execution.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution/stop_token.hpp>
#endif

TEST(stoptoken_inplace) {
    // section [stoptoken.inplace] is empty
}
