// src/beman/execution/tests/stopsource-inplace-general.test.cpp
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <test/execution.hpp>
#include <test/stop_token.hpp>
#if false && defined(BEMAN_HAS_MODULES) //-dk:TODO module disabled
import beman.execution;
#else
#include <beman/execution/stop_token.hpp>
#endif

TEST(stopsource_inplace_general) {
    // Reference: [stopsource.inplace.general]
    static_assert(::test_detail::stoppable_source<::test_std::inplace_stop_source>);
    ::test_std::inplace_stop_source source;
    static_assert(source.stop_possible());
    ::test::stop_source([] { return ::test_std::inplace_stop_source(); });
}
