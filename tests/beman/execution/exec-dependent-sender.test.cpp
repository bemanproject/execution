// src/beman/execution/tests/exec-dependent-sender.test.cpp            -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <concepts>
#include <test/execution.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution/detail/dependent_sender.hpp>
#include <beman/execution/detail/just.hpp>
#include <beman/execution/detail/let.hpp>
#include <beman/execution/detail/read_env.hpp>
#include <beman/execution/detail/then.hpp>
#include <beman/execution/detail/when_all.hpp>
#include <beman/execution/detail/when_all_with_variant.hpp>
#endif

TEST(exec_dependent_sender) {
    static_assert(test_std::dependent_sender<decltype(test_std::read_env(test_std::get_scheduler))>);
    static_assert(test_std::dependent_sender<decltype(test_std::let_value(
                      test_std::just(), []() noexcept { return test_std::read_env(test_std::get_scheduler); }))>);
    static_assert(
        test_std::dependent_sender<decltype(test_std::let_value(
            test_std::read_env(test_std::get_scheduler), [](auto sched) noexcept { return test_std::just(sched); }))>);
    static_assert(test_std::dependent_sender<decltype(test_std::read_env(test_std::get_scheduler) |
                                                      test_std::then([](auto sched) noexcept {}))>);
    static_assert(test_std::dependent_sender<decltype(test_std::let_value(
                                                          test_std::read_env(test_std::get_scheduler),
                                                          [](auto sched) noexcept { return test_std::just(sched); }) |
                                                      test_std::then([](auto sched) noexcept {}))>);
    static_assert(
        test_std::dependent_sender<decltype(test_std::when_all(test_std::read_env(test_std::get_scheduler)))>);
    static_assert(test_std::dependent_sender<decltype(test_std::when_all_with_variant(
                      test_std::read_env(test_std::get_scheduler)))>);
    static_assert(!test_std::dependent_sender<decltype(test_std::when_all(test_std::just()))>);
    static_assert(!test_std::dependent_sender<decltype(test_std::when_all_with_variant(test_std::just()))>);
    static_assert(test_std::dependent_sender<decltype(test_std::when_all(
                      test_std::just(), test_std::read_env(test_std::get_scheduler)))>);
}
