// src/beman/execution/tests/exec-bulk.test.cpp -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "beman/execution/detail/sync_wait.hpp"
#include <cstdlib>
#include <test/execution.hpp>
#include <beman/execution/detail/bulk.hpp>
#include <beman/execution/detail/just.hpp>

auto test_bulk() {
    auto b0 = test_std::bulk(test_std::just(), 1, [](int) {});

    static_assert(test_std::sender<decltype(b0)>);

    int counter = 0;

    auto b1 = test_std::bulk(test_std::just(), 5, [&](int i) { counter += i; });

    static_assert(test_std::sender<decltype(b1)>);
    test_std::sync_wait(b1);
    ASSERT(counter == 10);
}

TEST(exec_bulk) {

    try {

        test_bulk();

    } catch (...) {

        ASSERT(nullptr == "the bulk tests shouldn't throw");
    }

    return EXIT_SUCCESS;
}
