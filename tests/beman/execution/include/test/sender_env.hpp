// tests/beman/execution/include/test/sender_env.hpp                  -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_TESTS_BEMAN_EXECUTION_INCLUDE_TEST_SENDER_ENV
#define INCLUDED_TESTS_BEMAN_EXECUTION_INCLUDE_TEST_SENDER_ENV

#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#endif
#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution.hpp>
#endif
#include <test/execution.hpp>

// ----------------------------------------------------------------------------

namespace test {
struct test_forwardable_attr : test_std::forwarding_query_t {
    template <typename Env>
        requires requires(const test_forwardable_attr& attr, const Env& env) {
            { env.query(attr) } noexcept -> std::same_as<int>;
        }
    auto operator()(const Env& env) const noexcept -> int {
        return env.query(*this);
    }
};

struct test_non_forwardable_attr {
    template <typename Env>
        requires requires(const test_non_forwardable_attr& attr, const Env& env) {
            { env.query(attr) } noexcept -> std::same_as<int>;
        }
    auto operator()(const Env& env) const noexcept -> int {
        return env.query(*this);
    }
};

struct sender_env {
    using sender_concept        = test_std::sender_tag;
    using completion_signatures = test_std::completion_signatures<test_std::set_value_t()>;

    int value{};

    struct env {
        int  value{};
        auto query(const test_forwardable_attr&) const noexcept -> int { return this->value; }
        auto query(const test_non_forwardable_attr&) const noexcept -> int { return 2 * this->value; }
    };
    auto get_env() const noexcept -> env { return env{this->value}; }

    template <typename, typename...>
    static consteval auto get_completion_signatures() -> completion_signatures {
        return {};
    }
    template <test_std::receiver Receiver>
    auto connect(Receiver&&) && noexcept {
        return test_std::connect(test_std::just(), std::forward<Receiver>(Receiver{}));
    }
};

template <bool Expected>
void test_sender_env(int expected, auto attr, auto&& sender) {
    if constexpr (requires { attr(test_std::get_env(sender)); }) {
        ASSERT(attr(test_std::get_env(sender)) == expected);
    } else {
        static_assert(!Expected);
    }
}
} // namespace test

// ----------------------------------------------------------------------------

#endif
