// examples/doc_just.cpp                                              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <string>
#endif
#include <cassert>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution.hpp>
#endif
namespace ex = beman::execution;
using namespace std::string_literals;

int main() {
    auto result = ex::sync_wait(ex::just(17, "hello"s, true));
    assert(result);
    assert(*result == std::tuple(17, "hello"s, true));
}
