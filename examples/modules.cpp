// examples/modules.cpp                                               -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// FIXME: does not compile with g++-15 on OSX! CK
// #if defined(__APPLE__) && defined(__GNUC__)
// FIXME: does not seem to compile with g++-15 in Docker! DK
#if defined(__GNUC__)
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

import beman.execution;

namespace ex = beman::execution;

int main() {
    auto [result] = ex::sync_wait(ex::when_all(ex::just(std::string("hello, ")), ex::just(std::string("world"))) |
                                  ex::then([](const auto& s1, const auto& s2) { return s1 + s2; }))
                        .value_or(std::tuple(std::string("oops")));
    std::cout << "result='" << result << "'\n";
}
