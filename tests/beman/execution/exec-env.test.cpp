// tests/beman/execution/exec-env.test.cpp                            -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <memory>
#include <type_traits>
#include <test/execution.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution/detail/env.hpp>
#include <beman/execution/detail/get_allocator.hpp>
#include <beman/execution/detail/get_stop_token.hpp>
#include <beman/execution/detail/inplace_stop_source.hpp>
#include <beman/execution/detail/prop.hpp>
#endif

// ----------------------------------------------------------------------------

namespace {
struct extra_arg {
    int value{};
};

struct multi_arg_query_t {
    constexpr auto operator()(const auto& e) const noexcept(noexcept(e.query(*this))) -> decltype(e.query(*this)) {
        return e.query(*this);
    }
    constexpr auto operator()(const auto& e, extra_arg a) const noexcept(noexcept(e.query(*this, a)))
        -> decltype(e.query(*this, a)) {
        return e.query(*this, a);
    }
};
constexpr multi_arg_query_t multi_arg_query{};

struct multi_arg_env {
    auto query(const multi_arg_query_t&) const noexcept { return 42; }
    auto query(const multi_arg_query_t&, extra_arg a) const noexcept { return a.value; }
};
} // namespace

TEST(env) {
    test_std::inplace_stop_source    source{};
    [[maybe_unused]] test_std::env<> e0{};
    [[maybe_unused]] test_std::env   e1{test_std::prop(test_std::get_allocator, std::allocator<int>{})};
    [[maybe_unused]] test_std::env   e2{test_std::prop(test_std::get_allocator, std::allocator<int>{}),
                                        test_std::prop(test_std::get_stop_token, source.get_token())};
    [[maybe_unused]] auto            a1 = e1.query(test_std::get_allocator);
    [[maybe_unused]] auto            a2 = e2.query(test_std::get_allocator);
    [[maybe_unused]] auto            s2 = e2.query(test_std::get_stop_token);
    assert(s2 == source.get_token());

    static_assert(not std::is_assignable_v<test_std::env<>, test_std::env<>>);

    // P3826R5: env::query now accepts extra arguments (Args&&...)
    // The extra args are forwarded to the first matching sub-environment
    test_std::env e3{multi_arg_env{}};
    ASSERT(e3.query(multi_arg_query) == 42);
    ASSERT(e3.query(multi_arg_query, extra_arg{99}) == 99);

    // Test env with prop that accepts extra args
    test_std::env e4{test_std::prop(test_std::get_allocator, std::allocator<int>{}), multi_arg_env{}};
    ASSERT(e4.query(multi_arg_query) == 42);
    ASSERT(e4.query(multi_arg_query, extra_arg{7}) == 7);
}
