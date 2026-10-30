// examples/async_inclusive_scan.cpp                       -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <cstddef>
#include <algorithm>
#include <iterator>
#include <numeric>
#include <ranges>
#include <print>
#include <vector>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution.hpp>
#endif

namespace ex = beman::execution;

namespace {
template <ex::scheduler                                      Scheduler,
          std::forward_iterator                              ForwardIt,
          std::sentinel_for<ForwardIt>                       Sentinel,
          std::output_iterator<std::iter_value_t<ForwardIt>> OutputIt>
ex::sender auto
async_inclusive_scan(Scheduler scheduler, std::size_t tile_count, ForwardIt first, Sentinel last, OutputIt dest) {
    const std::size_t n         = std::ranges::distance(first, last);
    const std::size_t tile_size = (n + tile_count - 1) / tile_count;
    return ex::just(std::vector<int>(tile_count + 1)) | ex::let_value([=](std::vector<int>& partials) {
               return ex::schedule(scheduler) |
                      ex::bulk(ex::par,
                               tile_count,
                               [=, &partials](std::size_t i) {
                                   auto start = i * tile_size;
                                   auto end   = std::min(n, (i + 1) * tile_size);
                                   partials[i + 1] =
                                       *std::ranges::prev(std::inclusive_scan(std::ranges::next(first, start),
                                                                              std::ranges::next(first, end),
                                                                              std::ranges::next(dest, start)));
                               }) |
                      ex::then(
                          [&partials] { std::inclusive_scan(partials.begin(), partials.end(), partials.begin()); }) |
                      ex::bulk(ex::par,
                               tile_count,
                               [=, &partials](std::size_t i) {
                                   auto start = i * tile_size;
                                   auto end   = std::min(n, (i + 1) * tile_size);
                                   std::for_each(std::ranges::next(dest, start),
                                                 std::ranges::next(dest, end),
                                                 [&](int& e) { e = partials[i] + e; });
                               }) |
                      ex::then([=] { return std::ranges::next(dest, n); });
           });
}
} // namespace

auto main() -> int {
    constexpr std::size_t  n         = 32uz;
    ex::parallel_scheduler scheduler = ex::get_parallel_scheduler();
    std::vector<int>       input(n);
    std::vector<int>       output(input.size());
    std::iota(input.begin(), input.end(), 1); // NOLINT
    auto _ = ex::sync_wait(async_inclusive_scan(scheduler, 4, input.begin(), input.end(), output.begin())).value();
    std::println("{}", output);
}
