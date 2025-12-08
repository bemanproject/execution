// examples/modules.cpp                                               -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <version>
#include <iostream>
#include <string>
#endif

import beman_execution;

namespace ex = beman::execution;

int main() {
#ifdef USE_THIS_CODE
    auto [result] = ex::sync_wait(ex::when_all(ex::just(std::string("hello, ")), ex::just(std::string("world"))) |
                                  ex::then([](const auto& s1, const auto& s2) { return s1 + s2; }))
                        .value_or(std::tuple(std::string("oops")));
    std::cout << "result='" << result << "'\n";
#elif defined(BEMAN_HAS_IMPORT_STD)
    std::println("ex::version = {}", ex::version);
#else
    std::cout << "ex::version = " << ex::version << "\n";
#endif
}
