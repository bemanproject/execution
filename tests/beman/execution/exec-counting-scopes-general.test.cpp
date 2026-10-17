// tests/beman/execution/exec-counting-scopes-general.test.cpp        -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/detail/common.hpp>
#include <test/execution.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <concepts>
#include <tuple>
#include <type_traits>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution;
import beman.execution.detail;
#else
#include <beman/execution/detail/counting_scope.hpp>
#include <beman/execution/detail/inline_scheduler.hpp>
#include <beman/execution/detail/prop.hpp>
#endif

// ----------------------------------------------------------------------------

namespace {
auto test_scope_join() -> void {
    using scope_join_sender =
        std::invoke_result_t<test_detail::counting_scope_join_t, test_detail::counting_scope_base*>;
    static_assert(not test_std::sender_in<scope_join_sender, test_std::env<>>);
    using env_which_has_start_scheduler =
        decltype(test_std::prop{test_std::get_start_scheduler, test_std::inline_scheduler{}});
    static_assert(test_std::sender_in<scope_join_sender, env_which_has_start_scheduler>);
}

auto test_scope_state_type() -> void {
    using type = test_detail::counting_scope_base::state_t;

    test::use(type::unused);
    test::use(type::open);
    test::use(type::closed);
    test::use(type::open_and_joining);
    test::use(type::closed_and_joining);
    test::use(type::unused_and_closed);
    test::use(type::joined);
}
} // namespace

// ----------------------------------------------------------------------------

TEST(exec_counting_scopes_general) {
    test_scope_join();
    test_scope_state_type();
}
