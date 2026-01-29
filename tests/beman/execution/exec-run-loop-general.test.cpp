// src/beman/execution/tests/exec-run-loop-general.test.cpp         -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifdef BEMAN_USE_MODULE
import beman.execution;
#else
#include <beman/execution/detail/run_loop.hpp>
#include <beman/execution/detail/scheduler.hpp>
#endif
#include <test/execution.hpp>
#include <concepts>
#include <type_traits>

// ----------------------------------------------------------------------------

namespace {
auto use(auto&&...) {}
} // namespace

TEST(exec_run_loop_general) {
    static_assert(noexcept(test_std::run_loop()));
    static_assert(not std::move_constructible<test_std::run_loop>);
    test_std::run_loop rl;
    use(rl);

    static_assert(requires {
        { rl.get_scheduler() } -> test_std::scheduler;
    });
    static_assert(requires { rl.run(); });
    static_assert(requires { rl.finish(); });
}
