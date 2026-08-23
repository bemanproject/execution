// src/beman/execution/tests/exec-read-env.test.cpp                 -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <test/execution.hpp>
#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <concepts>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution;
import beman.execution.detail.join_env;
#else
#include <beman/execution/detail/read_env.hpp>
#include <beman/execution/detail/common.hpp>
#include <beman/execution/detail/dependent_sender.hpp>
#include <beman/execution/detail/get_domain.hpp>
#include <beman/execution/detail/inline_scheduler.hpp>
#include <beman/execution/detail/join_env.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/sender_in.hpp>
#include <beman/execution/detail/receiver.hpp>
#include <beman/execution/detail/connect.hpp>
#include <beman/execution/detail/start.hpp>
#include <beman/execution/detail/get_allocator.hpp>
#include <beman/execution/detail/get_scheduler.hpp>
#include <beman/execution/detail/get_stop_token.hpp>
#include <beman/execution/detail/sync_wait.hpp>
#include <beman/execution/detail/when_all.hpp>
#endif

// ----------------------------------------------------------------------------

namespace {
struct domain {
    int  value{};
    auto operator==(const domain&) const -> bool = default;
};

struct get_test_domain_t {
    template <typename Env>
    auto operator()(const Env& ev) const noexcept -> domain {
        return ev.query(*this);
    }
};
inline constexpr get_test_domain_t get_test_domain{};

struct env {
    int  value{};
    auto query(test_std::get_domain_t) const noexcept -> domain { return {this->value}; }
    auto query(get_test_domain_t) const noexcept -> domain { return {this->value}; }
};

struct receiver {
    using receiver_concept = test_std::receiver_tag;

    int   value{};
    int   expect{};
    bool* called{};

    auto set_value(domain d) && noexcept -> void {
        ASSERT(d == domain{this->expect});
        *this->called = true;
    }
    auto set_error(auto&&) && noexcept -> void {
        // NOLINTBEGIN(cert-dcl03-c,hicpp-static-assert,misc-static-assert)
        ASSERT(nullptr == "error function was incorrectly called");
        // NOLINTEND(cert-dcl03-c,hicpp-static-assert,misc-static-assert)
    }
    auto get_env() const noexcept -> env { return {this->value}; }
};

struct env1 {
    static auto query(test_std::get_allocator_t) noexcept { return std::allocator<char>{}; }
};

struct env2 {
    static auto query(test_std::get_scheduler_t) noexcept { return test_std::inline_scheduler{}; }
};

auto test_read_env_concept() -> void {
    static_assert(test_std::dependent_sender<decltype(test_std::read_env(test_std::get_domain))>);
    static_assert(test_std::dependent_sender<decltype(test_std::read_env(test_std::get_allocator))>);
    static_assert(test_std::sender_in<decltype(test_std::read_env(test_std::get_domain)), env>);

    static_assert(not test_std::sender_in<decltype(test_std::read_env(test_std::get_allocator)), env>);
    static_assert(test_std::sender_in<decltype(test_std::read_env(test_std::get_allocator)), env1>);
    static_assert(not test_std::sender_in<decltype(test_std::read_env(test_std::get_allocator)), env2>);

    static_assert(not test_std::sender_in<decltype(test_std::read_env(test_std::get_scheduler)), env>);
    static_assert(not test_std::sender_in<decltype(test_std::read_env(test_std::get_scheduler)), env1>);
    static_assert(test_std::sender_in<decltype(test_std::read_env(test_std::get_scheduler)), env2>);
}

auto test_read_env() -> void {
    static_assert(test_std::receiver<receiver>);
    ASSERT(domain{} == test_std::get_domain(env{17}));
    ASSERT(domain{} == test_std::get_domain(test_std::get_env(receiver{17, 0})));
    ASSERT(domain{17} == get_test_domain(env{17}));
    ASSERT(domain{17} == get_test_domain(test_std::get_env(receiver{17, 0})));
    auto sender{test_std::read_env(test_std::get_domain)};
    test::use(sender);
    static_assert(test_std::sender<decltype(sender)>);
    static_assert(test_std::sender_in<decltype(sender), env>);
    static_assert(
        std::same_as<test_std::completion_signatures<test_std::set_value_t(domain)
                                                     //-dk:TODO verify , test_std::set_error_t(std::exception_ptr)
                                                     >,
                     decltype(test_std::get_completion_signatures<decltype(sender), env>())>);

    bool called{};
    auto op1{test_std::connect(test_std::read_env(test_std::get_domain), receiver{17, 0, &called})};
    ASSERT(not called);
    test_std::start(op1);
    ASSERT(called);

    called = false;
    auto op2{test_std::connect(test_std::read_env(get_test_domain), receiver{17, 17, &called})};
    test_std::start(op2);
    ASSERT(called);
}

auto test_read_env_completions() -> void {
    auto r{test_std::read_env(test_std::get_stop_token)};
    test::check_type<test_std::completion_signatures<test_std::set_value_t(test_std::never_stop_token)>>(
        test_std::get_completion_signatures<decltype(r), test_std::env<>>());
    test::check_type<test_std::completion_signatures<test_std::set_value_t(test_std::never_stop_token)>>(
        test_std::get_completion_signatures<decltype(r), decltype(test_std::env{})>());
    test::check_type<test_std::completion_signatures<test_std::set_value_t(test_std::inplace_stop_token)>>(
        test_std::get_completion_signatures<decltype(r),
                                            decltype(test_std::env{
                                                test_std::prop{test_std::get_stop_token,
                                                               std::declval<test_std::inplace_stop_token>()}})>());
    test::check_type<test_std::completion_signatures<test_std::set_value_t(test_std::inplace_stop_token)>>(
        test_std::get_completion_signatures<
            decltype(r),
            decltype(test_detail::join_env(
                test_std::env{test_std::prop{test_std::get_stop_token, std::declval<test_std::inplace_stop_token>()}},
                test_std::env{
                    test_std::prop{test_std::get_stop_token, std::declval<test_std::never_stop_token>()}}))>());
    test::use(r);

    test_std::sync_wait(test_std::read_env(test_std::get_stop_token));
    test_std::sync_wait(test_std::when_all(test_std::read_env(test_std::get_stop_token)));
    test_std::sync_wait(test_std::when_all(test_std::read_env(test_std::get_scheduler)));
}

struct test_query_t {
    auto operator()(auto&& env) const noexcept { return env.query(*this); }
};
inline constexpr test_query_t test_query{};

template <bool Valid>
struct test_env {
    auto query(test_query_t) const noexcept {
        if constexpr (Valid) {
            return std::allocator<int>{};
        }
    }
};

auto test_read_env_check_types() -> void {
    test_std::read_env_t::impls_for::check_types<decltype(test_std::read_env(test_std::get_stop_token)),
                                                 test_std::env<>>();
    test_std::read_env_t::impls_for::check_types<decltype(test_std::read_env(test_query)), test_env<true>>();
#if 0
     test_std::read_env_t::impls_for::check_types<decltype(test_std::read_env(test_query)),
                                                  test_env<false>>();
     test_std::read_env_t::impls_for::check_types<decltype(test_std::read_env(test_std::get_allocator)),
                                                           test_std::env<>>();
#endif
}
} // namespace

TEST(exec_read_env) {
    static_assert(std::same_as<const test_std::read_env_t, decltype(test_std::read_env)>);
    test_read_env_concept();
    test_read_env();
    test_read_env_completions();
    test_read_env_check_types();
}
