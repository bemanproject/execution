// examples/modules.cpp                                               -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef WIN32
#undef BEMAN_HAS_IMPORT_STD
#endif

#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else

#include <version>
#include <iostream>
#include <string>
#include <variant>
#include <optional>
#include <tuple>

#endif

#if defined(__cpp_modules) && __cpp_modules < 201907L

#ifdef BEMAN_USE_MODULES
#warning "__cpp_modules seems not usable!"
import beman.execution;
#else
#include <beman/execution/execution.hpp>
#endif

#else

import beman.execution;

#endif

namespace ex = beman::execution;

int main() {
    auto [result] = ex::sync_wait(ex::when_all(ex::just(std::string("hello, ")), ex::just(std::string("world"))) |
                                  ex::then([](const auto& s1, const auto& s2) { return s1 + s2; }))
                        .value_or(std::tuple(std::string("oops")));
    std::cout << "result='" << result << "'\n";
}
