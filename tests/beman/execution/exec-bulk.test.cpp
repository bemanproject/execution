// src/beman/execution/tests/exec-bulk.test.cpp -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "beman/execution/detail/get_completion_signatures.hpp"
#include "beman/execution/detail/get_env.hpp"
#include "beman/execution/detail/sync_wait.hpp"
#include <cstdlib>
#include <test/execution.hpp>
#include <beman/execution/detail/bulk.hpp>
#include <beman/execution/detail/just.hpp>
#include <vector>

namespace {
auto test_bulk() {
    auto b0 = test_std::bulk(test_std::just(), 1, [](int) {});

    static_assert(test_std::sender<decltype(b0)>);
    auto b0_env         = test_std::get_env(b0);
    auto b0_completions = test_std::get_completion_signatures(b0, b0_env);
    static_assert(
        std::is_same_v<decltype(b0_completions),
                       beman::execution::completion_signatures<beman::execution::set_value_t(),
                                                               beman::execution::set_error_t(std::exception_ptr)> >,
        "Completion signatures do not match!");

    int counter = 0;

    auto b1 = test_std::bulk(test_std::just(), 5, [&](int i) { counter += i; });

    static_assert(test_std::sender<decltype(b1)>);
    auto b1_env         = test_std::get_env(b0);
    auto b1_completions = test_std::get_completion_signatures(b1, b1_env);
    static_assert(
        std::is_same_v<decltype(b1_completions),
                       beman::execution::completion_signatures<beman::execution::set_value_t(),
                                                               beman::execution::set_error_t(std::exception_ptr)> >,
        "Completion signatures do not match!");
    test_std::sync_wait(b1);
    ASSERT(counter == 10);

    std::vector<int> a{1, 2, 3, 4, 5, 6, 7, 8};
    std::vector<int> b{9, 10, 11, 13, 14, 15, 16, 17};

    std::vector<int> results(a.size(), 0);

    auto b2 = test_std::bulk(test_std::just(a), a.size(), [&](std::size_t index, const std::vector<int>& vec) {
        results[index] = vec[index] * b[index];
    });
    static_assert(test_std::sender<decltype(b2)>);
    auto b2_env         = test_std::get_env(b2);
    auto b2_completions = test_std::get_completion_signatures(b2, b2_env);
    static_assert(
        std::is_same_v<decltype(b2_completions),
                       beman::execution::completion_signatures<beman::execution::set_value_t(std::vector<int>),
                                                               beman::execution::set_error_t(std::exception_ptr)> >,
        "Completion signatures do not match!");
    test_std::sync_wait(b2);

    // Expected results: element-wise multiplication of a and b
    std::vector<int> expected{9, 20, 33, 52, 70, 90, 112, 136};

    for (size_t i = 0; i < results.size(); ++i) {
        ASSERT(results[i] == expected[i]);
    }
}

auto test_bulk_noexept() {
    auto b0             = test_std::bulk(test_std::just(), 1, [](int) noexcept {});
    auto b0_env         = test_std::get_env(b0);
    auto b0_completions = test_std::get_completion_signatures(b0, b0_env);
    static_assert(std::is_same_v<decltype(b0_completions),
                                 beman::execution::completion_signatures<beman::execution::set_value_t()> >,
                  "Completion signatures do not match!");
    static_assert(test_std::sender<decltype(b0)>);

    int counter = 0;

    auto b1 = test_std::bulk(test_std::just(), 5, [&](int i) noexcept { counter += i; });

    static_assert(test_std::sender<decltype(b1)>);
    auto b1_env         = test_std::get_env(b0);
    auto b1_completions = test_std::get_completion_signatures(b1, b1_env);
    static_assert(std::is_same_v<decltype(b1_completions),
                                 beman::execution::completion_signatures<beman::execution::set_value_t()> >,
                  "Completion signatures do not match!");
    test_std::sync_wait(b1);
    ASSERT(counter == 10);
}

} // namespace

TEST(exec_bulk) {

    try {

        test_bulk();
        test_bulk_noexept();

    } catch (...) {

        ASSERT(nullptr == "the bulk tests shouldn't throw");
    }

    return EXIT_SUCCESS;
}
