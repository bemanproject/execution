// src/beman/execution/tests/stopcallback-general.test.cpp
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/stop_token.hpp>
#include "test/execution.hpp"
#include <concepts>
#include <type_traits>

namespace {
auto test_stop_callback_interface() -> void {
    // Reference: [stopcallback.general] p1
    struct ThrowInit {};
    struct NothrowInit {};
    struct Callback {
        explicit Callback(ThrowInit) {}
        explicit Callback(NothrowInit) noexcept {}
        auto operator()() -> void {}
    };

    using CB = ::test_std::stop_callback<Callback>;
    const ::test_std::stop_token ctoken;

    static_assert(::std::same_as<Callback, CB::callback_type>);
    static_assert(not noexcept(CB(ctoken, ThrowInit())));
    static_assert(noexcept(CB(ctoken, NothrowInit())));
    static_assert(not noexcept(CB(::test_std::stop_token(), ThrowInit())));
    static_assert(noexcept(CB(::test_std::stop_token(), NothrowInit())));

    CB ccb(ctoken, ThrowInit());
    CB tcb{::test_std::stop_token(), ThrowInit()};

    static_assert(!::std::is_copy_constructible_v<CB>);
    static_assert(!::std::is_move_constructible_v<CB>);
    static_assert(!::std::is_copy_assignable_v<CB>);
    static_assert(!::std::is_move_assignable_v<CB>);

    ::test_std::stop_callback cb(ctoken, Callback(ThrowInit()));
}
} // namespace

TEST(stopcallback_general) { test_stop_callback_interface(); }
