// tests/beman/execution/exec-env.test.cpp                            -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/detail/env.hpp>
#include <beman/execution/detail/get_allocator.hpp>
#include <beman/execution/detail/get_stop_token.hpp>
#include <beman/execution/detail/inplace_stop_source.hpp>
#include <beman/execution/detail/prop.hpp>
#include <type_traits>
#include <test/execution.hpp>

namespace {}

// ----------------------------------------------------------------------------

int main() {
    test_std::inplace_stop_source  source{};
    test_std::env<>                e0{};
    [[maybe_unused]] test_std::env e1{test_std::prop(test_std::get_allocator, std::allocator<int>{})};
    [[maybe_unused]] test_std::env e2{test_std::prop(test_std::get_allocator, std::allocator<int>{}),
                                      test_std::prop(test_std::get_stop_token, source.get_token())};
    [[maybe_unused]] auto          a1 = e1.query(test_std::get_allocator);
    [[maybe_unused]] auto          a2 = e2.query(test_std::get_allocator);
    [[maybe_unused]] auto          s2 = e2.query(test_std::get_stop_token);
    assert(s2 == source.get_token());

    static_assert(not std::is_assignable_v<test_std::env<>, test_std::env<>>);
}
