// src/beman/execution/tests/stopsource-inplace-mem.test.cpp
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution/stop_token.hpp>
#endif
#include <test/execution.hpp>

namespace {
auto test_inplace_stop_source_get_token() -> void {
    // Plan:
    // - Given two inplace_stop_sources.
    // - When getting a stop_token each using get_token().
    // - Then these token compare unequal to each other but equal to
    //   other tokens get from the respective source.
    // Reference: [stopsource.inplace.mem] p1

    ::test_std::inplace_stop_source source1;
    ::test_std::inplace_stop_source source2;

    auto token1{source1.get_token()};
    auto token2{source2.get_token()};

    ASSERT(token1 != token2);
    ASSERT(token1 == source1.get_token());
    ASSERT(token2 == source2.get_token());
}

auto test_inplace_stop_source_stop_requested() -> void {
    // Plan:
    // - Given an inplace_stop_source.
    // - When getting using stop_requested() it yields false.
    // - Then after calling request_stop() stop_requested()
    //   yields true.
    // Reference: [stopsource.inplace.mem] p2

    ::test_std::inplace_stop_source source;

    ASSERT(source.stop_requested() == false);
    source.request_stop();
    ASSERT(source.stop_requested() == true);
}

auto test_inplace_stop_source_request_stop() -> void {
    // Plan:
    // - Given an inplace_stop_source.
    // - When getting using stop_requested() it yields false
    //   and registered callbacks are executed..
    // - Then after calling request_stop() stop_requested()
    //   yields true.
    // Reference: [stopsource.inplace.mem] p4

    bool flag1{false};
    bool flag2{false};

    ::test_std::inplace_stop_source source;
    auto                            callback1 = [&flag1] { flag1 = true; };
    auto                            callback2 = [&flag2] { flag2 = true; };
    ::test_std::stop_callback_for_t<::test_std::inplace_stop_token, decltype(callback1)> cb1(source.get_token(),
                                                                                             callback1);
    ::test_std::stop_callback_for_t<::test_std::inplace_stop_token, decltype(callback2)> cb2(source.get_token(),
                                                                                             callback2);

    ASSERT(source.stop_requested() == false);
    ASSERT(flag1 == false);
    ASSERT(flag2 == false);

    source.request_stop();

    ASSERT(source.stop_requested() == true);
    ASSERT(flag1 == true);
    ASSERT(flag2 == true);
}
} // namespace

TEST(stopsource_inplace_mem) {
    test_inplace_stop_source_get_token();
    test_inplace_stop_source_stop_requested();
    test_inplace_stop_source_request_stop();
}
