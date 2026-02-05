// src/beman/execution/tests/stoptoken-general.test.cpp
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <test/stop_token.hpp>
#if defined(BEMAN_HAS_MODULES) //-dk:TODO module disabled
import beman.execution;
#else
#include <beman/execution/stop_token.hpp>
#endif

TEST(stoptoken_general) {
    // [stoptoken.general] p1:
    static_assert(::test_std::stoppable_token<::test_std::stop_token>);
}
