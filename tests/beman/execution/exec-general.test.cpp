// src/beman/execution/tests/exec-general.test.cpp                  -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution/execution.hpp>
#endif
#include <beman/execution/detail/movable_value.hpp>
#include <beman/execution/detail/matching_sig.hpp>
#include <beman/execution/detail/as_except_ptr.hpp>
#include <test/execution.hpp>
#include <concepts>
#include <exception>
#include <system_error>

// ----------------------------------------------------------------------------

namespace {
struct error : std::exception {
    int value;
    explicit error(int v) : value(v) {}
};

struct non_movable {
    non_movable(non_movable&&)                         = delete;
    non_movable(const non_movable&)                    = delete;
    ~non_movable()                                     = default;
    auto operator=(non_movable&&) -> non_movable&      = delete;
    auto operator=(const non_movable&) -> non_movable& = delete;
};
struct non_copyable {
    non_copyable(non_copyable&&)                         = default;
    non_copyable(const non_copyable&)                    = delete;
    ~non_copyable()                                      = default;
    auto operator=(non_copyable&&) -> non_copyable&      = default;
    auto operator=(const non_copyable&) -> non_copyable& = delete;
};

auto test_movable_value() -> void {
    static_assert(test_detail::movable_value<int>);
    static_assert(test_detail::movable_value<int&>);
    static_assert(test_detail::movable_value<const int&>);

    static_assert(not test_detail::movable_value<non_movable>);
    static_assert(not test_detail::movable_value<const non_copyable>);
    // clang-format off
    static_assert(not test_detail::movable_value<int[1]>);    // NOLINT(hicpp-avoid-c-arrays)
    static_assert(not test_detail::movable_value<int(&)[1]>); // NOLINT(hicpp-avoid-c-arrays)
    // clang-format on
}

auto test_matching_sig() -> void {
    static_assert(test_detail::matching_sig<int(), int()>);
    static_assert(not test_detail::matching_sig<bool(), int()>);

    static_assert(test_detail::matching_sig<int(int), int(int)>);
    static_assert(test_detail::matching_sig<int(int&&), int(int)>);

    static_assert(test_detail::matching_sig<int(int&&, bool&, double), int(int, bool&, double&&)>);
}

auto test_as_except_ptr() -> void {
    std::exception_ptr       ep(std::make_exception_ptr(42));
    std::exception_ptr       mep(ep);
    const std::exception_ptr cep(std::make_exception_ptr(42));
    static_assert(std::same_as<std::exception_ptr&, decltype(test_detail::as_except_ptr(ep))>);
    static_assert(std::same_as<std::exception_ptr&&, decltype(test_detail::as_except_ptr(std::move(ep)))>);
    static_assert(std::same_as<const std::exception_ptr&, decltype(test_detail::as_except_ptr(cep))>);

    ASSERT(ep == test_detail::as_except_ptr(ep));
    ASSERT(ep == mep);
    ASSERT(ep == test_detail::as_except_ptr(std::move(mep)));
    ASSERT(cep == test_detail::as_except_ptr(cep));
    //-dk:TODO set up death test: test_detail::as_except_ptr(::std::exception_ptr());

    static_assert(std::same_as<std::exception_ptr, decltype(test_detail::as_except_ptr(error{17}))>);
    try {
        std::rethrow_exception(test_detail::as_except_ptr(error{17}));
    } catch (error& e) {
        ASSERT(e.value == 17);
    } catch (...) {
        // NOLINTNEXTLINE(cert-dcl03-c,hicpp-static-assert,misc-static-assert)
        ASSERT(nullptr == "wrong exception type produced by as_except_ptr for random error");
    }

    std::error_code errc(17, ::std::system_category());
    static_assert(std::same_as<std::exception_ptr, decltype(test_detail::as_except_ptr(errc))>);
    try {
        std::rethrow_exception(test_detail::as_except_ptr(errc));
    } catch (std::system_error& e) {
        ASSERT(e.code() == errc);
    } catch (...) {
        // NOLINTNEXTLINE(cert-dcl03-c,hicpp-static-assert,misc-static-assert)
        ASSERT(nullptr == "wrong exception type produced by as_except_ptr for error code");
    }
}
} // namespace

// ----------------------------------------------------------------------------

TEST(exec_general) {
    // Tests for [exec.general]
    // p1 doesn't contain anything testable
    // p2 doesn't contain anything testable
    // p3 doesn't contain anything testable
    // p4.1 needs to be tested by uses of MANDATE-NOTHROW: 13 places
    test_movable_value(); // p4.2
    test_matching_sig();  // p4.3
    test_as_except_ptr(); // p4.4
}
