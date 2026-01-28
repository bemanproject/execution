// src/beman/execution/tests/stoptoken-general.test.cpp
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifdef BEMAN_USE_MODULE
import beman.execution;
#else
#include <beman/execution/stop_token.hpp>
#endif
#include "test/execution.hpp"
#include "test/stop_token.hpp"

TEST(stoptoken_general) {
    // [stoptoken.general] p1:
    static_assert(::test_std::stoppable_token<::test_std::stop_token>);
}
