// src/beman/execution/tests/stoptoken-inplace-members.test.cpp
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifdef BEMAN_USE_MODULE
import beman.execution;
#else
#include <beman/execution/stop_token.hpp>
#endif
#include "test/execution.hpp"

namespace {
auto test_inplace_stop_token_swap() -> void {
    // Plan:
    // - Given two inplace_stop_token objects which don't compare equal but
    //   comparing equal to their respective sources token.
    // - When swapping the two objects.
    // - Then they compare equal to the respective opposite source's  token.
    // Reference: [stoptoken.inplace.members] p1

    ::test_std::inplace_stop_source source1, source2;
    auto                            token1(source1.get_token());
    auto                            token2(source2.get_token());

    ASSERT(token1 == source1.get_token());
    ASSERT(token2 == source2.get_token());
    ASSERT(token1 != token2);

    token1.swap(token2);

    ASSERT(token1 == source2.get_token());
    ASSERT(token2 == source1.get_token());
}

auto test_inplace_stop_token_stop_requested() -> void {
    // Plan:
    // - Given a connected and a disconnected inplace_stop_token.
    // - Then using stop_requested() it return false.
    // - When requesteing stop for the inplace_stop_source.
    // - Then the connected inplace_stop_token returns true from
    //   stop_requested().
    // Reference: [stoptoken.inplace.members] p2

    ::test_std::inplace_stop_source source;
    ::test_std::inplace_stop_token  connected{source.get_token()};
    ::test_std::inplace_stop_token  disconnected;

    ASSERT(connected.stop_requested() == false);
    ASSERT(disconnected.stop_requested() == false);

    source.request_stop();

    ASSERT(connected.stop_requested() == true);
    ASSERT(disconnected.stop_requested() == false);
}

auto test_inplace_stop_token_stop_possible() -> void {
    // Plan:
    // - Given a connected and a disconnected inplace_stop_token.
    // - Then using stop_possible() it returns false for the
    //   disconnected inplace_stop_token and true for the connected
    //   one.
    // - When requesteing stop for the inplace_stop_source.
    // - Then the results of stop_possible() didn't change.
    // Reference: [stoptoken.inplace.members] p4

    ::test_std::inplace_stop_source source;
    ::test_std::inplace_stop_token  connected{source.get_token()};
    ::test_std::inplace_stop_token  disconnected;

    ASSERT(connected.stop_possible() == true);
    ASSERT(disconnected.stop_possible() == false);

    source.request_stop();

    ASSERT(connected.stop_possible() == true);
    ASSERT(disconnected.stop_requested() == false);
}
} // namespace

TEST(stoptoken_inplace_members) {
    test_inplace_stop_token_swap();
    test_inplace_stop_token_stop_requested();
    test_inplace_stop_token_stop_possible();
}
