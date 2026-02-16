// src/beman/execution/tests/exec-set-stopped.test.cpp              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <concepts>
#include <utility>
#include <test/execution.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution/execution.hpp>
#endif

namespace {
struct receiver {
    bool* called;
    auto  set_stopped() noexcept -> void { *called = true; }
};

struct receiver_throwing {
    auto set_stopped() -> void {}
};

template <typename R>
auto test_callable() -> void {
    // can be called with non-const rvalue receiver
    static_assert(requires { test_std::set_stopped(std::declval<R>()); });
    // clang-format off
    static_assert(requires { test_std::set_stopped(std::declval<R&&>()); });
    // clang-format on

    // cannot be called with const or lvalue receiver
    static_assert(not requires { test_std::set_stopped(std::declval<const R>()); });
    // clang-format off
    static_assert(not requires { test_std::set_stopped(std::declval<const R&&>()); });
    // clang-format on
    static_assert(not requires { test_std::set_stopped(std::declval<R&>()); });
    static_assert(not requires { test_std::set_stopped(std::declval<const R&>()); });
    static_assert(not requires { test_std::set_stopped(std::declval<volatile R&>()); });
    static_assert(not requires { test_std::set_stopped(std::declval<const volatile R&>()); });
}

template <typename R>
auto test_noexcept() -> void {
    static_assert(not requires { test_std::set_stopped(R()); });
}
} // namespace

// ----------------------------------------------------------------------------

TEST(exec_set_stopped) {
    static_assert(std::semiregular<test_std::set_stopped_t>);
    static_assert(std::same_as<const test_std::set_stopped_t, decltype(test_std::set_stopped)>);

    test_callable<receiver>();
    test_noexcept<receiver_throwing>();

    bool called{false};
    ASSERT(not called);
    test_std::set_stopped(receiver{&called});
    ASSERT(called);
}
