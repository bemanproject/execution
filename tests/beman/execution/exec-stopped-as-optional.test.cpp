// src/beman/execution/tests/exec-stopped-as-error.test.cpp             -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <concepts>
#include <optional>
#include <system_error>
#include <test/execution.hpp>
#include <test/optional_sender.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
import beman.execution.detail;
#else
#include <beman/execution/detail/just.hpp>
#include <beman/execution/detail/receiver.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/stopped_as_optional.hpp>
#include <beman/execution/detail/sync_wait.hpp>
#endif

// ----------------------------------------------------------------------------
namespace {
auto test_stopped_as_optional() -> void {
    test_std::sender auto sndr1 = test_std::just(42) | test_std::stopped_as_optional();
    using sigs_of_sndr1         = test_std::completion_signatures_of_t<decltype(sndr1), test_detail::sync_wait_env>;
    static_assert(
        std::same_as<sigs_of_sndr1, test_std::completion_signatures<test_std::set_value_t(std::optional<int>)>>);
    auto [i] = test_std::sync_wait(std::move(sndr1)).value();
    ASSERT(i == 42);

    test_std::sender auto sndr2 = test_std::stopped_as_optional(test::optional_sender<int>{});
    using sigs_of_sndr2 = test_std::completion_signatures_of_t<decltype(sndr2), test_std::detail::sync_wait_env>;
    static_assert(
        std::same_as<sigs_of_sndr2, test_std::completion_signatures<test_std::set_value_t(std::optional<int>)>>);
    auto [opt] = test_std::sync_wait(std::move(sndr2)).value();
    ASSERT(!opt.has_value());
}

} // namespace
// ----------------------------------------------------------------------------

TEST(exec_stopped_as_optional) { test_stopped_as_optional(); }
