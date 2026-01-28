// src/beman/execution/tests/stopsource-mem.test.cpp
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifdef BEMAN_USE_MODULE
import beman.execution;
#else
#include <beman/execution/stop_token.hpp>
#endif
#include "test/execution.hpp"

namespace {
auto test_stopsource_swap() -> void {
    // Plan:
    //  - Given two engaged stop sources.
    //  - When two corresponding tokens are obtained.
    //  - Then after swapping the source the tokens compare suitably
    //    equal to the sources's get_token()
    // Reference: [stopsource.mem] p1

    ::test_std::stop_source source1;
    ::test_std::stop_source source2;

    auto token1{source1.get_token()};
    auto token2{source2.get_token()};
    ASSERT(token1 == source1.get_token());
    ASSERT(token1 != source2.get_token());
    ASSERT(token2 != source1.get_token());
    ASSERT(token2 == source2.get_token());

    source1.swap(source2);

    ASSERT(token1 != source1.get_token());
    ASSERT(token1 == source2.get_token());
    ASSERT(token2 == source1.get_token());
    ASSERT(token2 != source2.get_token());
}

auto test_stopsource_get_token() -> void {
    // Plan:
    //  - Given an engaged and a disengaged stop sources.
    //  - When two corresponding tokens are obtained.
    //  - Then get_token() for the engaged stop source returns
    //    a non-default constructed token while the disengaged
    //    source returns a token comparing equal to a default
    //    constructed token.
    // Reference: [stopsource.mem] p2

    ::test_std::stop_source engaged;
    ::test_std::stop_source disengaged(::test_std::nostopstate);

    ASSERT(engaged.get_token() == engaged.get_token());
    ASSERT(engaged.get_token() != ::test_std::stop_token());
    ASSERT(engaged.get_token() != disengaged.get_token());
    ASSERT(disengaged.get_token() == ::test_std::stop_token());
}

auto test_stopsource_stop_possible() -> void {
    // Plan:
    //  - Given an engaged and a disengaged stop sources.
    //  - When using stop_possible()
    //  - Then the result is true for the engaged stop source (both
    //    before request_stop() was called and after) while
    //    the result is false for the disengaged stop source.
    // Reference: [stopsource.mem] p3

    ::test_std::stop_source engaged;
    ::test_std::stop_source disengaged(::test_std::nostopstate);

    ASSERT(engaged.stop_possible());
    engaged.request_stop();
    ASSERT(engaged.stop_possible());

    ASSERT(not disengaged.stop_possible());
    disengaged.request_stop();
    ASSERT(not disengaged.stop_possible());
}

auto test_stopsource_stop_requested() -> void {
    // Plan:
    //  - Given an engaged and a disengaged stop sources.
    //  - When using stop_requested()
    //  - Then the result is true for the engaged stop source
    //    after a call to request_stop() but false before.
    //    The result is false for the disengaged stop source.
    // Reference: [stopsource.mem] p4

    ::test_std::stop_source engaged;
    ::test_std::stop_source disengaged(::test_std::nostopstate);

    ASSERT(not engaged.stop_requested());
    engaged.request_stop();
    ASSERT(engaged.stop_requested());

    ASSERT(not disengaged.stop_requested());
    disengaged.request_stop();
    ASSERT(not disengaged.stop_requested());
}

auto test_stopsource_request_stop() -> void {
    // Plan:
    //  - Given an engaged and a disengaged stop sources.
    //  - When using request_stop()
    //  - Then the result is true for the first call of the engaged
    //    stop source. All subsequent calls on the engaged stop source
    //    and all calls on the disengaged stop source return false.
    // Reference: [stopsource.mem] p5 dnd [stopsource.concepts] p8

    ::test_std::stop_source engaged;
    ::test_std::stop_source disengaged(::test_std::nostopstate);

    ASSERT(not engaged.stop_requested());
    auto res1{engaged.request_stop()};
    ASSERT(res1);
    ASSERT(engaged.stop_requested());
    auto res2{engaged.request_stop()};
    ASSERT(res2 == false);
    ASSERT(engaged.stop_requested());

    ASSERT(not disengaged.stop_requested());
    auto res3{disengaged.request_stop()};
    ASSERT(res3 == false);
    ASSERT(not disengaged.stop_requested());
    auto res4{disengaged.request_stop()};
    ASSERT(res4 == false);
    ASSERT(not disengaged.stop_requested());
}
} // namespace

TEST(stopsource_mem) {
    try {
        test_stopsource_swap();
        test_stopsource_get_token();
        test_stopsource_stop_possible();
        test_stopsource_stop_requested();
        test_stopsource_request_stop();

    } catch (...) {
        // NOLINTNEXTLINE(cert-dcl03-c,hicpp-static-assert,misc-static-assert)
        ASSERT(nullptr == "the stop source tests shouldn't throw");
    }
}
