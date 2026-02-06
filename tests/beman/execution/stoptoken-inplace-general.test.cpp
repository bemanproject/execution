// src/beman/execution/tests/stoptoken-inplace-general.test.cpp
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <type_traits>
#include <cassert>
#include <test/execution.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution/stop_token.hpp>
#endif

TEST(stoptoken_inplace_general) {
    // Reference [stoptoken.inplace.general]
    static_assert(::test_std::stoppable_token<::test_std::inplace_stop_token>);
    static_assert(::std::is_swappable_v<::test_std::inplace_stop_token>);

    ::test_std::inplace_stop_token token, token2;
    ASSERT(false == token.stop_requested());
    ASSERT(false == token.stop_possible());
    token.swap(token2);
    ASSERT(token == token2);

    struct Callback {
        explicit Callback(int*) {}
        auto operator()() const -> void {}
    };
    ::test_std::stop_callback_for_t<::test_std::inplace_stop_token, Callback> cb(token, nullptr);
}
