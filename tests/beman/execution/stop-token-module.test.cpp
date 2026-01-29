// tests/beman/execution/stop-token-module.test.cpp                 -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <test/execution.hpp>
import beman.execution;

// ----------------------------------------------------------------------------

TEST(stop_token_modules) {
    // [stoptoken.concepts], stop token concepts
    static_assert(not test_std::stoppable_token<int>);
    static_assert(not test_std::unstoppable_token<int>);

    // [stoptoken], class stop_token
    test::use_type<test_std::stop_token>();

    // [stopsource], class stop_source
    test::use_type<test_std::stop_source>();

    // [stopcallback], class template stop_callback
    test::use_template<test_std::stop_callback>();

    // [stoptoken.never], class never_stop_token
    test::use_type<test_std::never_stop_token>();

    // [stoptoken.inplace], class inplace_stop_token
    test::use_type<test_std::inplace_stop_token>();

    // [stopsource.inplace], class inplace_stop_source
    test::use_type<test_std::inplace_stop_source>();

    // [stopcallback.inplace], class template inplace_stop_callback
    test::use_template<test_std::inplace_stop_callback>();
    test::use_template<test_std::stop_callback_for_t>();
}
