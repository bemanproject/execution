// tests/beman/execution/exec-unstoppable.test.cpp                    -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <concepts>
#endif
#include <test/execution.hpp>
#include <test/sender_env.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution.hpp>
#include <beman/stop_token.hpp>
#endif

// ----------------------------------------------------------------------------

namespace {

auto test_unstoppable() -> void {
    auto sender = test_std::read_env(test_std::get_stop_token) | test_std::then([](auto stop_token) noexcept {
                      return test_std::unstoppable_token<decltype(stop_token)>;
                  });
    test_std::inplace_stop_source source;
    auto [result1] = *test_std::sync_wait(
        test_std::write_env(sender, test_std::env{test_std::prop{test_std::get_stop_token, source.get_token()}}));
    ASSERT(result1 == false);
    auto [result2] = *test_std::sync_wait(test_std::write_env(
        test_std::unstoppable(sender), test_std::env{test_std::prop{test_std::get_stop_token, source.get_token()}}));
    ASSERT(result2 == true);
}

auto test_unstoppable_attributes() -> void {
    test::sender_env s{42};
    test::test_sender_env<true>(42, test::test_forwardable_attr{}, s);
    test::test_sender_env<true>(84, test::test_non_forwardable_attr{}, s);
    test::test_sender_env<true>(42, test::test_forwardable_attr{}, test_std::unstoppable(s));
    test::test_sender_env<false>(84, test::test_non_forwardable_attr{}, test_std::unstoppable(s));
}
} // namespace

TEST(exec_unstoppable) {
    test_unstoppable();
    test_unstoppable_attributes();
}
