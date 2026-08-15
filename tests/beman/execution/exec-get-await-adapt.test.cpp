// tests/beman/execution/exec-get-await-adapt.test.cpp                -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <test/execution.hpp>
#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <concepts>
#include <utility>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution.hpp>
#endif

// ----------------------------------------------------------------------------

namespace {
struct awaiter {
    int  value{};
    auto operator==(const awaiter&) const noexcept -> bool = default;
    auto await_ready() const noexcept -> bool { return true; }
    auto await_suspend(std::coroutine_handle<>) const noexcept -> void {}
    auto await_resume() const noexcept -> void {}
};

struct env {
    int  value{};
    auto query(beman::execution::get_await_completion_adaptor_t) const noexcept -> awaiter { return {this->value}; }
};
} // namespace

TEST(exec_get_await_adapt) {
    ASSERT(beman::execution::get_await_completion_adaptor(env{17}).value == 17);
    [](const auto& env) {
        ASSERT(!requires { beman::execution::get_await_completion_adaptor(env); });
    }(test_std::env<>{});
}
