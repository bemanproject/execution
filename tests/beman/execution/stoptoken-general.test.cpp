// src/beman/execution/tests/stoptoken-general.test.cpp
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "test/execution.hpp"
#include "test/stop_token.hpp"
#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution/stop_token.hpp>
#endif

TEST(stoptoken_general) {
    // [stoptoken.general] p1:
    static_assert(::test_std::stoppable_token<::test_std::stop_token>);
}
