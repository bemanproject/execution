// examples/intro-2-hello-async.cpp                                   -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/execution.hpp>
#include "intro-timer.hpp"
#include <chrono>
#include <iostream>
#include <string>
#include <tuple>

namespace ex = ::beman::execution;
using namespace std::string_literals;
using namespace std::chrono_literals;

// ----------------------------------------------------------------------------
// Please see the explanation in docs/intro-examples.md for an explanation.

int main() {
    std::cout << std::unitbuf;
    intro::timer timer;

    // clang-format off
     auto [result] = ex::sync_wait(
         ex::when_all(
             timer.run(),
             ex::when_all(
                 timer.resume_after(30ms)
                     | ex::then([] { std::cout << "h\n"; return std::string("hello"); }),
                 timer.resume_after(10ms)
                     | ex::then([] { std::cout << ",\n"; return std::string(", "); }),
                 timer.resume_after(20ms)
                     | ex::then([] { std::cout << "w\n"; return std::string("world"); })
             ) | ex::then([](auto const& s1, auto const& s2, auto const& s3) { return s1 + s2 + s3; })
         )
     ).value_or(std::tuple(std::string("")));
    // clang-format on

    std::cout << result << "\n";
}
