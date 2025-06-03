// tests/beman/execution/exec-prop.test.cpp                           -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/detail/prop.hpp>
#include <beman/execution/detail/forwarding_query.hpp>
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
        auto query(test_query_t const&) const noexcept { return 42; }
    };

    template <typename Env, typename Value>
    auto test_prop(Env&& env, Value&& value) {
        static_assert(requires{ { test_query(env) } noexcept; });
        ASSERT(test_query(env) == value);
    }
}

TEST(exec_prop) {
    test_prop(env{}, 42);
    test_prop(test_std::prop(test_query, 42), 42);
    [[maybe_unused]] auto p{test_std::prop(test_query, 2.5)};
    static_assert(not std::is_assignable_v<decltype(p), decltype(p)>);
}

