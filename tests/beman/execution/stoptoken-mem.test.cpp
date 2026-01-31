// src/beman/execution/tests/stoptoken-mem.test.cpp
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution/stop_token.hpp>
#endif
#include <array>
#include <memory>
#include "test/execution.hpp"
#include "test/stop_token.hpp"

namespace {
auto test_stop_token_swap() -> void {
    // Plan:
    // - Given two pairs of stop_tokens compare equal within the pair
    //   but are different between the pairse.
    // - When swapping the one element of each pair.
    // - Then the tokens compare equal to the respective other member.
    // Reference: [stoptoken.mem] p1

    ::test_std::stop_source                 s0, s1;
    ::std::array<::test_std::stop_token, 2> pair0 = {s0.get_token(), s0.get_token()};
    ::std::array<::test_std::stop_token, 2> pair1 = {s1.get_token(), s1.get_token()};

    ASSERT(pair0[0] == pair0[1]);
    ASSERT(pair1[0] == pair1[1]);
    ASSERT(pair0[0] != pair1[1]);
    ASSERT(pair0[1] != pair1[0]);

    pair0[1].swap(pair1[1]);

    ASSERT(pair0[0] != pair0[1]);
    ASSERT(pair1[0] != pair1[1]);
    ASSERT(pair0[0] == pair1[1]);
    ASSERT(pair0[1] == pair1[0]);
}

auto test_stop_token_stop_requested() -> void {
    // Plan:
    // - Given a stop source and its stop token with stop_requested() == false.
    //   Also, a disengaged stop_token needs to have stop_requested() == false.
    // - When requesting stop on the source.
    // - Then the token's stop_requested() should be true. The disengaged stop
    //   token's stop_requested() remains false.
    // Reference: [stoptoken.mem] p2

    ::test_std::stop_source source;
    ::test_std::stop_token  engaged{source.get_token()};
    ::test_std::stop_token  disengaged;
    ASSERT(engaged.stop_requested() == false);
    ASSERT(disengaged.stop_requested() == false);

    source.request_stop();

    ASSERT(engaged.stop_requested() == true);
    ASSERT(disengaged.stop_requested() == false);
}

auto test_stop_token_stop_possible() -> void {
    // Plan:
    // - Given a three stop sources:
    //     - remain_source    - which sticks around
    //     - unstopped_source - which is destroyed before stopped
    //     - stopped_source   - which is destroyed after stopped
    // - When getting a stop token for each of them (prior to destruction)
    // - Then the token's stop_possible() should be:
    //     - false for a disengaged token
    //     - true for the remain_source
    //     - false for the unstopped source
    //     - true for the stopped source
    // Reference: [stoptoken.mem] p3

    auto remain_source(::std::make_unique<::test_std::stop_source>());
    auto unstopped_source(::std::make_unique<::test_std::stop_source>());
    auto stopped_source(::std::make_unique<::test_std::stop_source>());

    auto disengaged_token{::test_std::stop_token()};
    auto remain_token{remain_source->get_token()};

    auto unstopped_token{unstopped_source->get_token()};
    unstopped_source.reset();

    auto stopped_token{stopped_source->get_token()};
    stopped_source->request_stop();
    stopped_source.reset();

    ASSERT(disengaged_token.stop_possible() == false);
    ASSERT(remain_token.stop_possible() == true);
    ASSERT(unstopped_token.stop_possible() == false);
    ASSERT(stopped_token.stop_possible() == true);
}
} // namespace

TEST(stoptoken_mem) {
    static_assert(::test_std::stoppable_token<::test_std::stop_token>);

    try {
        test_stop_token_swap();
        test_stop_token_stop_requested();
        test_stop_token_stop_possible();
    } catch (...) {
        // NOLINTBEGIN(cert-dcl03-c,hicpp-static-assert,misc-static-assert)
        ASSERT(nullptr == "the stop token tests are not expected to throw");
        // NOLINTEND(cert-dcl03-c,hicpp-static-assert,misc-static-assert)
    }
}
