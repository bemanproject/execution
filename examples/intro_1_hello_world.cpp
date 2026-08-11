// examples/intro_1_hello_world.cpp                                   -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <iostream>
#include <optional>
#include <string>
#include <tuple>
#include <variant>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution.hpp>
#endif

namespace ex = ::beman::execution;
using namespace std::string_literals;

// ----------------------------------------------------------------------------
// Please see the explanation in docs/intro_examples.md for an explanation.

int main() {
    // clang-format off
#if !defined(__GNUC__) || defined(__clang__) || (__GNUC__ > 15) || !defined(BEMAN_HAS_MODULES)
    auto [result] =
    ex::sync_wait(
        ex::when_all(
            ex::just("hello, "s),
            ex::just("world"s)
        ) | ex::then([](auto const& s1, auto const& s2) { return s1 + s2; })
        ).value_or(std::tuple(""s))
        ;
#endif
    // clang-format on

    // std::cout << std::get<0>(v) << '\n';
}
