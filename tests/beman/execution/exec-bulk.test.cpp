// src/beman/execution/tests/exec-bulk.test.cpp -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cstdlib>
#include <tuple>
#include <vector>
#include <execution>
#include <test/execution.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution/detail/bulk.hpp>
#include <beman/execution/detail/just.hpp>
#include <beman/execution/detail/get_completion_signatures.hpp>
#include <beman/execution/detail/get_env.hpp>
#include <beman/execution/detail/sync_wait.hpp>
#endif

namespace {

auto test_bulk() {
    auto b0 = test_std::bulk(test_std::just(), std::execution::seq, 1, [](int) {});

    static_assert(test_std::sender<decltype(b0)>);
    auto b0_env         = test_std::get_env(b0);
    auto b0_completions = test_std::get_completion_signatures<decltype(b0), decltype(b0_env)>();
    static_assert(
        std::is_same_v<decltype(b0_completions),
                       beman::execution::completion_signatures<beman::execution::set_value_t(),
                                                               beman::execution::set_error_t(std::exception_ptr)>>,
        "Completion signatures do not match!");

    int counter = 0;

    auto b1 = test_std::bulk(test_std::just(), std::execution::seq, 5, [&](int i) { counter += i; });

    static_assert(test_std::sender<decltype(b1)>);
    auto b1_env         = test_std::get_env(b0);
    auto b1_completions = test_std::get_completion_signatures<decltype(b1), decltype(b1_env)>();
    static_assert(
        std::is_same_v<decltype(b1_completions),
                       beman::execution::completion_signatures<beman::execution::set_value_t(),
                                                               beman::execution::set_error_t(std::exception_ptr)>>,
        "Completion signatures do not match!");
    test_std::sync_wait(b1);
    ASSERT(counter == 10);

    std::vector<int> a{1, 2, 3, 4, 5, 6, 7, 8};
    std::vector<int> b{9, 10, 11, 13, 14, 15, 16, 17};

    std::vector<int> results(a.size(), 0);

    auto b2 = test_std::bulk(
        test_std::just(a), std::execution::seq, a.size(), [&](std::size_t index, const std::vector<int>& vec) {
            results[index] = vec[index] * b[index];
        });
    static_assert(test_std::sender<decltype(b2)>);
    auto b2_env         = test_std::get_env(b2);
    auto b2_completions = test_std::get_completion_signatures<decltype(b2), decltype(b2_env)>();
    static_assert(
        std::is_same_v<decltype(b2_completions),
                       beman::execution::completion_signatures<beman::execution::set_value_t(std::vector<int>),
                                                               beman::execution::set_error_t(std::exception_ptr)>>,
        "Completion signatures do not match!");
    test_std::sync_wait(b2);

    std::vector<int> expected{9, 20, 33, 52, 70, 90, 112, 136};

    for (::std::size_t i = 0; i < results.size(); ++i) {
        ASSERT(results[i] == expected[i]);
    }
}

auto test_bulk_noexcept() {
    auto b0             = test_std::bulk(test_std::just(), std::execution::seq, 1, [](int) noexcept {});
    auto b0_env         = test_std::get_env(b0);
    auto b0_completions = test_std::get_completion_signatures<decltype(b0), decltype(b0_env)>();
    static_assert(std::is_same_v<decltype(b0_completions),
                                 beman::execution::completion_signatures<beman::execution::set_value_t()>>,
                  "Completion signatures do not match!");
    static_assert(test_std::sender<decltype(b0)>);

    int counter = 0;

    auto b1 = test_std::bulk(test_std::just(), std::execution::seq, 5, [&](int i) noexcept { counter += i; });

    static_assert(test_std::sender<decltype(b1)>);
    auto b1_env         = test_std::get_env(b0);
    auto b1_completions = test_std::get_completion_signatures<decltype(b1), decltype(b1_env)>();
    static_assert(std::is_same_v<decltype(b1_completions),
                                 beman::execution::completion_signatures<beman::execution::set_value_t()>>,
                  "Completion signatures do not match!");
    test_std::sync_wait(b1);
    ASSERT(counter == 10);
}

auto test_bulk_pipeable() {
    auto b0 = test_std::just() | test_std::bulk(std::execution::seq, 1, [](int) {});

    static_assert(test_std::sender<decltype(b0)>);
    auto b0_env         = test_std::get_env(b0);
    auto b0_completions = test_std::get_completion_signatures<decltype(b0), decltype(b0_env)>();
    static_assert(
        std::is_same_v<decltype(b0_completions),
                       beman::execution::completion_signatures<beman::execution::set_value_t(),
                                                               beman::execution::set_error_t(std::exception_ptr)>>,
        "Completion signatures do not match!");

    int counter = 0;

    auto b1 = test_std::just() | test_std::bulk(std::execution::seq, 5, [&](int i) { counter += i; });

    static_assert(test_std::sender<decltype(b1)>);
    auto b1_env         = test_std::get_env(b0);
    auto b1_completions = test_std::get_completion_signatures<decltype(b1), decltype(b1_env)>();
    static_assert(
        std::is_same_v<decltype(b1_completions),
                       beman::execution::completion_signatures<beman::execution::set_value_t(),
                                                               beman::execution::set_error_t(std::exception_ptr)>>,
        "Completion signatures do not match!");
    test_std::sync_wait(b1);
    ASSERT(counter == 10);

    std::vector<int> a{1, 2, 3, 4, 5, 6, 7, 8};
    std::vector<int> b{9, 10, 11, 13, 14, 15, 16, 17};

    std::vector<int> results(a.size(), 0);

    auto b2 = test_std::just(a) |
              test_std::bulk(std::execution::seq, a.size(), [&](std::size_t index, const std::vector<int>& vec) {
                  results[index] = vec[index] * b[index];
              });

    static_assert(test_std::sender<decltype(b2)>);
    auto b2_env         = test_std::get_env(b2);
    auto b2_completions = test_std::get_completion_signatures<decltype(b2), decltype(b2_env)>();
    static_assert(
        std::is_same_v<decltype(b2_completions),
                       beman::execution::completion_signatures<beman::execution::set_value_t(std::vector<int>),
                                                               beman::execution::set_error_t(std::exception_ptr)>>,
        "Completion signatures do not match!");
    test_std::sync_wait(b2);

    std::vector<int> expected{9, 20, 33, 52, 70, 90, 112, 136};

    for (::std::size_t i = 0; i < results.size(); ++i) {
        ASSERT(results[i] == expected[i]);
    }
}

auto test_bulk_chunked() {
    int counter = 0;

    auto b0 = test_std::bulk_chunked(test_std::just(), std::execution::seq, 5, [&](int begin, int end) {
        for (int i = begin; i < end; ++i) {
            counter += i;
        }
    });

    static_assert(test_std::sender<decltype(b0)>);
    test_std::sync_wait(b0);
    ASSERT(counter == 10);
}

auto test_bulk_chunked_with_values() {
    std::vector<int> a{1, 2, 3, 4};
    std::vector<int> results(a.size(), 0);

    auto b0 = test_std::bulk_chunked(test_std::just(a),
                                     std::execution::seq,
                                     a.size(),
                                     [&](std::size_t begin, std::size_t end, const std::vector<int>& vec) {
                                         for (std::size_t i = begin; i < end; ++i) {
                                             results[i] = vec[i] * 2;
                                         }
                                     });

    static_assert(test_std::sender<decltype(b0)>);
    test_std::sync_wait(b0);

    std::vector<int> expected{2, 4, 6, 8};
    for (std::size_t i = 0; i < results.size(); ++i) {
        ASSERT(results[i] == expected[i]);
    }
}

auto test_bulk_chunked_pipeable() {
    int counter = 0;

    auto b0 = test_std::just() | test_std::bulk_chunked(std::execution::seq, 5, [&](int begin, int end) {
                  for (int i = begin; i < end; ++i) {
                      counter += i;
                  }
              });

    static_assert(test_std::sender<decltype(b0)>);
    test_std::sync_wait(b0);
    ASSERT(counter == 10);
}

auto test_bulk_unchunked() {
    int counter = 0;

    auto b0 = test_std::bulk_unchunked(test_std::just(), std::execution::seq, 5, [&](int i) { counter += i; });

    static_assert(test_std::sender<decltype(b0)>);
    test_std::sync_wait(b0);
    ASSERT(counter == 10);
}

auto test_bulk_unchunked_with_values() {
    std::vector<int> a{1, 2, 3, 4};
    std::vector<int> results(a.size(), 0);

    auto b0 = test_std::bulk_unchunked(
        test_std::just(a), std::execution::seq, a.size(), [&](std::size_t index, const std::vector<int>& vec) {
            results[index] = vec[index] * 3;
        });

    static_assert(test_std::sender<decltype(b0)>);
    test_std::sync_wait(b0);

    std::vector<int> expected{3, 6, 9, 12};
    for (std::size_t i = 0; i < results.size(); ++i) {
        ASSERT(results[i] == expected[i]);
    }
}

auto test_bulk_unchunked_pipeable() {
    int counter = 0;

    auto b0 = test_std::just() | test_std::bulk_unchunked(std::execution::seq, 5, [&](int i) { counter += i; });

    static_assert(test_std::sender<decltype(b0)>);
    test_std::sync_wait(b0);
    ASSERT(counter == 10);
}

auto test_execution_policies() {
    int counter = 0;
    test_std::sync_wait(test_std::bulk(test_std::just(), std::execution::par, 5, [&](int i) { counter += i; }));
    ASSERT(counter == 10);

    counter = 0;
    test_std::sync_wait(test_std::bulk(test_std::just(), std::execution::par_unseq, 5, [&](int i) { counter += i; }));
    ASSERT(counter == 10);

    counter = 0;
    test_std::sync_wait(test_std::bulk(test_std::just(), std::execution::unseq, 5, [&](int i) { counter += i; }));
    ASSERT(counter == 10);
}

auto test_bulk_shape_zero() {
    int counter = 0;
    test_std::sync_wait(test_std::bulk(test_std::just(), std::execution::seq, 0, [&](int) { counter++; }));
    ASSERT(counter == 0);

    test_std::sync_wait(
        test_std::bulk_chunked(test_std::just(), std::execution::seq, 0, [&](int, int) { counter++; }));
    ASSERT(counter == 0);

    test_std::sync_wait(test_std::bulk_unchunked(test_std::just(), std::execution::seq, 0, [&](int) { counter++; }));
    ASSERT(counter == 0);
}

auto test_bulk_exception_handling() {
    bool error_caught = false;

    auto sndr = test_std::bulk(test_std::just(), std::execution::seq, 5, [](int i) {
        if (i == 3)
            throw std::runtime_error("test error");
    });

    try {
        test_std::sync_wait(sndr);
    } catch (...) {
        error_caught = true;
    }
    ASSERT(error_caught);
}

auto test_bulk_chunked_exception_handling() {
    bool error_caught = false;

    auto sndr = test_std::bulk_chunked(
        test_std::just(), std::execution::seq, 5, [](int, int) { throw std::runtime_error("chunked error"); });

    try {
        test_std::sync_wait(sndr);
    } catch (...) {
        error_caught = true;
    }
    ASSERT(error_caught);
}

auto test_bulk_unchunked_exception_handling() {
    bool error_caught = false;

    auto sndr = test_std::bulk_unchunked(test_std::just(), std::execution::seq, 5, [](int i) {
        if (i == 2)
            throw std::runtime_error("unchunked error");
    });

    try {
        test_std::sync_wait(sndr);
    } catch (...) {
        error_caught = true;
    }
    ASSERT(error_caught);
}

auto test_bulk_chunked_noexcept() {
    auto b0             = test_std::bulk_chunked(test_std::just(), std::execution::seq, 1, [](int, int) noexcept {});
    auto b0_env         = test_std::get_env(b0);
    auto b0_completions = test_std::get_completion_signatures<decltype(b0), decltype(b0_env)>();
    static_assert(std::is_same_v<decltype(b0_completions),
                                 beman::execution::completion_signatures<beman::execution::set_value_t()>>,
                  "Chunked noexcept completion signatures do not match!");
}

auto test_bulk_unchunked_noexcept() {
    auto b0             = test_std::bulk_unchunked(test_std::just(), std::execution::seq, 1, [](int) noexcept {});
    auto b0_env         = test_std::get_env(b0);
    auto b0_completions = test_std::get_completion_signatures<decltype(b0), decltype(b0_env)>();
    static_assert(std::is_same_v<decltype(b0_completions),
                                 beman::execution::completion_signatures<beman::execution::set_value_t()>>,
                  "Unchunked noexcept completion signatures do not match!");
}

auto test_bulk_shape_one() {
    int counter = 0;
    test_std::sync_wait(test_std::bulk(test_std::just(), std::execution::seq, 1, [&](int i) {
        ASSERT(i == 0);
        counter++;
    }));
    ASSERT(counter == 1);

    counter = 0;
    test_std::sync_wait(test_std::bulk_chunked(test_std::just(), std::execution::seq, 1, [&](int begin, int end) {
        ASSERT(begin == 0);
        ASSERT(end == 1);
        counter++;
    }));
    ASSERT(counter == 1);

    counter = 0;
    test_std::sync_wait(test_std::bulk_unchunked(test_std::just(), std::execution::seq, 1, [&](int i) {
        ASSERT(i == 0);
        counter++;
    }));
    ASSERT(counter == 1);
}

auto test_bulk_chunked_covers_full_range() {
    std::size_t seen_begin = 999;
    std::size_t seen_end   = 999;
    int         call_count = 0;

    test_std::sync_wait(test_std::bulk_chunked(
        test_std::just(), std::execution::seq, std::size_t(10), [&](std::size_t begin, std::size_t end) {
            seen_begin = begin;
            seen_end   = end;
            call_count++;
        }));

    ASSERT(call_count == 1);
    ASSERT(seen_begin == 0);
    ASSERT(seen_end == 10);
}

auto test_bulk_multiple_values() {
    int sum_a = 0;
    int sum_b = 0;

    test_std::sync_wait(test_std::bulk(test_std::just(10, 20), std::execution::seq, 3, [&](int i, int a, int b) {
        sum_a += a;
        sum_b += b + i;
    }));

    ASSERT(sum_a == 30);
    ASSERT(sum_b == 60 + 0 + 1 + 2);
}

} // namespace

TEST(exec_bulk) {

    try {

        test_bulk();
        test_bulk_noexcept();
        test_bulk_pipeable();
        test_bulk_chunked();
        test_bulk_chunked_with_values();
        test_bulk_chunked_pipeable();
        test_bulk_unchunked();
        test_bulk_unchunked_with_values();
        test_bulk_unchunked_pipeable();
        test_execution_policies();
        test_bulk_shape_zero();
        test_bulk_exception_handling();
        test_bulk_chunked_exception_handling();
        test_bulk_unchunked_exception_handling();
        test_bulk_chunked_noexcept();
        test_bulk_unchunked_noexcept();
        test_bulk_shape_one();
        test_bulk_chunked_covers_full_range();
        test_bulk_multiple_values();

    } catch (...) {

        ASSERT(nullptr == +"the bulk tests shouldn't throw");
    }

    return EXIT_SUCCESS;
}
