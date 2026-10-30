// examples/sleep_sort.cpp                                 -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <chrono>
#include <print>
#include <queue>
#include <vector>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution.hpp>
#endif
#include "intro_timer.hpp"

namespace ex = beman::execution;

namespace {
auto sleep_sort(std::span<const int> input) -> ex::task<std::vector<int>> {
    intro::timer     timer;
    std::vector<int> result;
    result.reserve(input.size());
    ex::simple_counting_scope scope;
    for (auto i : input) {
        ex::spawn(timer.resume_after(std::chrono::milliseconds(i)) |
                      ex::then([i, &result]() noexcept { result.push_back(i); }),
                  scope.get_token());
    }
    auto sch = co_await ex::read_env(ex::get_start_scheduler);
    co_await ex::when_all(timer.run() | ex::write_env(ex::prop{ex::get_delegation_scheduler, sch}), scope.join());
    co_return result;
}
} // namespace

auto main() -> int {
    auto        tick = std::chrono::steady_clock::now();
    std::vector input{5, 10, 1, 3, 4, 2, 9, 11, 8, 6, 7, 3, 4};
    auto [result] = ex::sync_wait(sleep_sort(input)).value();
    auto tock     = std::chrono::steady_clock::now();
    std::println("result = {}, take {}", result, std::chrono::duration_cast<std::chrono::milliseconds>(tock - tick));
}
