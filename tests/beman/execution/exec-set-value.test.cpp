// src/beman/execution/tests/exec-set-value.test.cpp                -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <utility>
#include <test/execution.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution/execution.hpp>
#endif

// ----------------------------------------------------------------------------

namespace {
template <int>
struct arg {};
struct throws {
    throws() = default;
    throws(throws&&) noexcept(false) {}
    throws(const throws&) noexcept(false) {}
    ~throws() = default;
    auto operator=(throws&&) noexcept(false) -> throws& { return *this; }
    auto operator=(const throws&) noexcept(false) -> throws& { return *this; } // NOLINT(cert-oop54-cpp)
};
struct receiver {
    template <typename... T>
    auto set_value(T&&...) noexcept -> void {}
    auto set_value(throws) noexcept -> void {} // NOLINT(performance-unnecessary-value-param)
    auto set_value(int i, bool b, double d) noexcept {
        ASSERT(i == 42);
        ASSERT(b == true);
        ASSERT(d == 123.75);
    }
};

template <typename R, typename... Args>
auto test_callable(Args&&... args) -> void {
    // can be called with non-const rvalue receiver
    static_assert(requires { test_std::set_value(std::declval<R>(), args...); });
    // clang-format off
    static_assert(requires { test_std::set_value(std::declval<R&&>(), args...); });
    // clang-format on

    // cannot be called with const or lvalue receiver
    static_assert(not requires { test_std::set_value(std::declval<const R>(), args...); });
    // clang-format off
    static_assert(not requires { test_std::set_value(std::declval<const R&&>(), args...); });
    // clang-format on
    static_assert(not requires { test_std::set_value(std::declval<R&>(), args...); });
    static_assert(not requires { test_std::set_value(std::declval<const R&>(), args...); });
    static_assert(not requires { test_std::set_value(std::declval<volatile R&>(), args...); });
    static_assert(not requires { test_std::set_value(std::declval<const volatile R&>(), args...); });
}

template <typename R>
auto test_noexcept() {
    static_assert(requires { test_std::set_value(std::declval<R>()); });
    static_assert(requires { test_std::set_value(std::declval<R>(), arg<0>()); });
    static_assert(not requires { test_std::set_value(std::declval<R>(), throws()); });
}
} // namespace

TEST(exec_set_value) {
    static_assert(std::semiregular<test_std::set_value_t>);
    static_assert(std::same_as<const test_std::set_value_t, decltype(test_std::set_value)>);

    test_callable<receiver>();
    test_callable<receiver>(arg<0>());
    test_callable<receiver>(arg<0>(), arg<1>());
    test_callable<receiver>(arg<0>(), arg<1>(), arg<2>());

    test_std::set_value(receiver{}, 1);
    test_std::set_value(receiver{}, arg<0>(), arg<1>(), arg<2>(), arg<3>(), arg<4>());
    test_std::set_value(receiver{}, 42, true, 123.75);

    test_noexcept<receiver>();
}
