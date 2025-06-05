// tests/beman/execution/exec-nest.test.cpp                           -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/detail/nest.hpp>
#include <test/execution.hpp>
#include <concepts>

// ----------------------------------------------------------------------------

namespace {
struct sender {
    using sender_concept = test_std::sender_t;
};
static_assert(test_std::sender<sender>);
static_assert(test_std::sender<sender&>);
static_assert(test_std::sender<const sender&>);
} // namespace

TEST(exec_nest) {
    static_assert(std::same_as<const test_std::nest_t, decltype(test_std::nest)>);

    sender sndr{};
    int    token{};
    // test_std::nest(sndr, token);
}
