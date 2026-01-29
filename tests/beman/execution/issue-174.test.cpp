// tests/beman/execution/issue-174.test.cpp                            *-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifdef BEMAN_USE_MODULE
import beman.execution;
#else
#include <beman/execution/execution.hpp>
#endif
#include <test/execution.hpp>
#include <thread>
#include <utility>

namespace ex = beman::execution;

namespace {
struct thread_loop : ex::run_loop {
    std::thread thread{[this] { this->run(); }};
    ~thread_loop() {
        this->finish();
        this->thread.join();
    }
};
} // namespace

TEST(issue174) {
    thread_loop ex_context1;
    thread_loop ex_context2;

    ex::sync_wait(ex::just() | ex::then([] {}) | ex::continues_on(ex_context1.get_scheduler()) | ex::then([] {}) |
                  ex::continues_on(ex_context2.get_scheduler()) | ex::then([] {}));
}
