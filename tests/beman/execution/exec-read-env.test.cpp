// src/beman/execution/tests/exec-read-env.test.cpp                 -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifdef BEMAN_USE_MODULE
import beman.execution;
#else
#include <beman/execution/detail/read_env.hpp>
#include <beman/execution/detail/common.hpp>
#include <beman/execution/detail/get_domain.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/sender_in.hpp>
#include <beman/execution/detail/receiver.hpp>
#include <beman/execution/detail/connect.hpp>
#include <beman/execution/detail/start.hpp>
#include <beman/execution/detail/get_stop_token.hpp>
#endif
#include <test/execution.hpp>
#include <concepts>

// ----------------------------------------------------------------------------

namespace {
struct domain {
    int  value{};
    auto operator==(const domain&) const -> bool = default;
};

struct env {
    int  value{};
    auto query(test_std::get_domain_t) const noexcept -> domain { return {this->value}; }
};

struct receiver {
    using receiver_concept = test_std::receiver_t;

    int   value{};
    bool* called{};

    auto set_value(domain d) && noexcept -> void {
        ASSERT(d == domain{this->value});
        *this->called = true;
    }
    auto set_error(auto&&) && noexcept -> void {
        // NOLINTBEGIN(cert-dcl03-c,hicpp-static-assert,misc-static-assert)
        ASSERT(nullptr == "error function was incorrectly called");
        // NOLINTEND(cert-dcl03-c,hicpp-static-assert,misc-static-assert)
    }
    auto get_env() const noexcept -> env { return {this->value}; }
};

auto test_read_env() -> void {
    static_assert(test_std::receiver<receiver>);
    ASSERT(domain{17} == test_std::get_domain(env{17}));
    ASSERT(domain{17} == test_std::get_domain(test_std::get_env(receiver{17})));
    auto sender{test_std::read_env(test_std::get_domain)};
    test::use(sender);
    static_assert(test_std::sender<decltype(sender)>);
    static_assert(test_std::sender_in<decltype(sender), env>);
    static_assert(
        std::same_as<test_std::completion_signatures<test_std::set_value_t(domain)
                                                     //-dk:TODO verify , test_std::set_error_t(std::exception_ptr)
                                                     >,
                     decltype(test_std::get_completion_signatures(sender, env{}))>);

    bool called{};
    auto op{test_std::connect(test_std::read_env(test_std::get_domain), receiver{17, &called})};
    test::use(op);
    ASSERT(not called);
    test_std::start(op);
    ASSERT(called);
}

auto test_read_env_completions() -> void {
    auto r{test_std::read_env(test_std::get_stop_token)};
    test::check_type<test_std::completion_signatures<test_std::set_value_t(test_std::never_stop_token)> >(
        test_std::get_completion_signatures(r, test_std::env<>{}));
    test::use(r);
}
} // namespace

TEST(exec_read_env) {
    static_assert(std::same_as<const test_std::read_env_t, decltype(test_std::read_env)>);
    test_read_env();
    test_read_env_completions();
}
