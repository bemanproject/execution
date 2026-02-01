// src/beman/execution/tests/stopsource-general.test.cpp
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution/stop_token.hpp>
#endif
#include <test/execution.hpp>
#include <test/stop_token.hpp>
#include <cassert>

TEST(stopsource_general) {
    try {
        // Reference: [stopsource.general] p1.
        test::stop_source([] { return ::test_std::stop_source(); });

        ::test_std::stop_source source1;
        ::test_std::stop_source source2{::test_std::nostopstate};
        static_assert(noexcept(::test_std::stop_source(::test_std::nostopstate_t())));
        try {
            ASSERT(not source2.stop_possible());
            ASSERT(not source2.stop_requested());
        } catch (...) {
            // NOLINTNEXTLINE(cert-dcl03-c,hicpp-static-assert,misc-static-assert)
            ASSERT(nullptr == "the stop source should not throw");
        }

        static_assert(noexcept(source1.swap(source2)));
        static_assert(noexcept(source1.stop_requested()));
        static_assert(noexcept(source1.stop_possible()));
        static_assert(noexcept(source1.request_stop()));
    } catch (...) {
        // NOLINTNEXTLINE(cert-dcl03-c,hicpp-static-assert,misc-static-assert)
        ASSERT(nullptr == "stop source tests shouldn't throw");
    }
}
