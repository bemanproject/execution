// src/beman/execution/tests/stopsource-inplace-cons.test.cpp
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <test/execution.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution/stop_token.hpp>
#endif

TEST(stopsource_inplace_cons) {
    // Plan:
    // - Given a default constructed inplace_stop_source.
    // - Then stop_requested() yields true.
    // Reference: [stopsource.inplace.general] p2

    ::test_std::inplace_stop_source source;
    ASSERT(source.stop_requested() == false);
}
