// tests/beman/execution/exec-schedule-from.test.cpp                  -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <test/execution.hpp>
#include <test/sender_env.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <concepts>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution/detail/schedule_from.hpp>
#include <beman/execution.hpp>
#endif

// ----------------------------------------------------------------------------

namespace {

auto test_schedule_from() {
    auto [a, b] = *test_std::sync_wait(test_std::schedule_from(test_std::just(42, true)));
    ASSERT(a == 42);
    ASSERT(b);
}

auto test_schedule_from_attributes() {
    test::sender_env s{42};
    test::test_sender_env<true>(42, test::test_forwardable_attr{}, s);
    test::test_sender_env<true>(84, test::test_non_forwardable_attr{}, s);
    test::test_sender_env<true>(42, test::test_forwardable_attr{}, test_std::schedule_from(s));
    test::test_sender_env<false>(84, test::test_non_forwardable_attr{}, test_std::write_env(s));
}
} // namespace

TEST(exec_schedule_from) {
    test_schedule_from();
    test_schedule_from_attributes();
}
