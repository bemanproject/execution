// tests/beman/execution/exec-prop.test.cpp                           -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifdef BEMAN_USE_MODULE
import beman.execution;
#else
#include <beman/execution/detail/prop.hpp>
#include <beman/execution/detail/forwarding_query.hpp>
#endif
#include <type_traits>
#include <concepts>
#include <test/execution.hpp>

// ----------------------------------------------------------------------------

namespace {
constexpr struct test_query_t {
    template <typename Env>
        requires requires(const test_query_t& self, const Env& e) { e.query(self); }
    decltype(auto) operator()(const Env& e) const noexcept(noexcept(e.query(*this))) {
        return e.query(*this);
    }
    constexpr auto query(const test_std::forwarding_query_t&) const noexcept -> bool { return true; }
} test_query{};

struct env {
    auto query(const test_query_t&) const noexcept { return 42; }
};

template <typename Env, typename Value>
auto test_prop(Env&& env, Value&& value) {
    static_assert(requires {
        { test_query(env) } noexcept;
    });
    ASSERT(test_query(env) == value);
}

} // namespace

TEST(exec_prop) {
    test_prop(env{}, 42);
    test_prop(test_std::prop(test_query, 42), 42);
    auto                          p0{test_std::prop(test_query, 2.5)};
    decltype(p0)                  p1 = test_std::prop(test_query, 2.5);
    [[maybe_unused]] decltype(p0) p2(p0);
    static_assert(not std::is_assignable_v<decltype(p0), decltype(p0)>);
    static_assert(not std::is_assignable_v<decltype(p1), std::add_rvalue_reference_t<decltype(p0)>>);
}
