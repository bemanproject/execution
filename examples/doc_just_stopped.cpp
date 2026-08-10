// examples/doc_just_stopped.cpp                                      -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <system_error>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution.hpp>
#endif
namespace ex = beman::execution;

namespace {
void use(auto&&...) {}
} // namespace

int main() {
    bool stopped{false};

    auto result = ex::sync_wait(ex::just_stopped() | ex::upon_stopped([&] { stopped = true; }));
    use(result, stopped);
    assert(result);
    assert(stopped);
}
