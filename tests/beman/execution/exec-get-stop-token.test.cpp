// src/beman/execution/tests/exec-get-stop-token.test.cpp           -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <test/execution.hpp>
#include <concepts>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution/execution.hpp>
#include <beman/execution/stop_token.hpp>
#endif

// ----------------------------------------------------------------------------

namespace {
struct non_stop_token {};
struct stop_token {
    template <typename CB>
    struct callback_type {};
    auto stop_requested() const noexcept -> bool { return {}; }
    auto stop_possible() const noexcept -> bool { return {}; }
    auto operator==(const stop_token&) const noexcept -> bool = default;
};

struct no_get_stop_token {};

struct non_const_get_stop_token {
    auto query(const test_std::get_stop_token_t&) noexcept -> stop_token { return {}; }
};
template <bool Noexcept>
struct has_get_stop_token {
    auto query(const test_std::get_stop_token_t&) const noexcept(Noexcept) -> stop_token { return {}; }
};

struct inconsistent_get_stop_token {
    auto query(const test_std::get_stop_token_t&) const noexcept -> stop_token { return {}; }
    auto query(const test_std::get_stop_token_t&) noexcept -> non_stop_token { return {}; }
};

template <typename Result, typename Object>
auto test_get_stop_token(Object&& object) {
    static_assert(requires { test_std::get_stop_token(object); });
    static_assert(std::same_as<Result, decltype(test_std::get_stop_token(object))>);
}
} // namespace

TEST(exec_get_stop_token) {
    static_assert(std::same_as<const test_std::get_stop_token_t, decltype(test_std::get_stop_token)>);
    static_assert(test_std::forwarding_query(test_std::get_stop_token));

    test_get_stop_token<test_std::never_stop_token>(no_get_stop_token());
    test_get_stop_token<stop_token>(has_get_stop_token<true>());
    test_get_stop_token<test_std::never_stop_token>(has_get_stop_token<false>());
    test_get_stop_token<test_std::never_stop_token>(non_const_get_stop_token());
    test_get_stop_token<stop_token>(inconsistent_get_stop_token());

    static_assert(test_std::stoppable_token<stop_token>);
}
