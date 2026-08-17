// examples/playground.cpp                                            -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <iostream>
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

// ----------------------------------------------------------------------------

int main() {
#if !defined(__GNUC__) || defined(__clang__) || (__GNUC__ > 15) || !defined(BEMAN_HAS_MODULES)
    auto [result] = ex::sync_wait(ex::when_all(ex::just(std::string("hello, ")), ex::just(std::string("world"))) |
                                  ex::then([](const auto& s1, const auto& s2) { return s1 + s2; }))
                        .value_or(std::tuple(std::string("oops")));
    std::cout << "result='" << result << "'\n";
#endif
}
