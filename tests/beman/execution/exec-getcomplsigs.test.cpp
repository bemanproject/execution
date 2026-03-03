// src/beman/execution/tests/exec-getcomplsigs.test.cpp             -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <concepts>
#include <test/execution.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
import beman.execution.detail.get_domain_late; //-dk:TODO remove when get_domain_late is removed
#else
#include <beman/execution/execution.hpp>
#endif

// ----------------------------------------------------------------------------

namespace {
struct arg {};
using signatures = test_std::completion_signatures<test_std::set_value_t(arg)>;
struct env {};

struct other_arg {};
using other_signatures = test_std::completion_signatures<test_std::set_value_t(other_arg)>;
struct other_env {};

template <typename>
struct no_signatures_sender {
    using sender_concept = test_std::sender_t;
};

struct sender_with_typedef {
    using sender_concept        = test_std::sender_t;
    using completion_signatures = signatures;
};

struct sender_from_domain {
    using sender_concept = test_std::sender_t;
    template <typename...>
    static consteval auto get_completion_signatures() noexcept {
        return signatures();
    }
};

struct sender_with_get_completion_signatures {
    using sender_concept = test_std::sender_t;
    template <typename, typename... Env>
    static consteval auto get_completion_signatures() noexcept {
        if constexpr ((std::same_as<other_env, Env> && ... && true)) {
            return other_signatures{};
        } else {
            return signatures{};
        }
    }
};

struct domain {
    auto transform_sender(auto&&, auto&&...) const noexcept -> sender_from_domain { return {}; }
};

struct env_with_domain {
    auto query(const test_std::get_domain_t&) const noexcept -> domain { return {}; }
};

template <typename T>
auto test_get_completion_signatures() {
    static_assert(test_std::sender<no_signatures_sender<T>>);
    //-dk:TODO static_assert(not requires { test_std::get_completion_signatures<no_signatures_sender<T>, env>(); });

    static_assert(test_std::sender<sender_with_typedef>);
    //-dk:TODO static_assert(not test_std::sender_in<sender_with_typedef, env>);

    static_assert(test_std::sender<sender_with_get_completion_signatures>);
    static_assert(requires {
        {
            test_std::get_completion_signatures<sender_with_get_completion_signatures, env>()
        } -> std::same_as<signatures>;
    });
    static_assert(requires {
        {
            test_std::get_completion_signatures<sender_with_get_completion_signatures, other_env>()
        } -> std::same_as<other_signatures>;
    });

    static_assert(std::same_as<domain, decltype(test_std::get_domain(env_with_domain{}))>);
    static_assert(
        std::same_as<sender_from_domain,
                     decltype(test_std::transform_sender(domain{}, no_signatures_sender<int>{}, env_with_domain{}))>);
    static_assert(std::same_as<sender_from_domain,
                               decltype(::beman::execution::transform_sender(
                                   ::beman::execution::detail::get_domain_late(
                                       ::std::declval<no_signatures_sender<int>>(), env_with_domain{}),
                                   no_signatures_sender<int>{},
                                   env_with_domain{}))>);
    static_assert(std::same_as<signatures,
                               decltype(test_std::get_completion_signatures<sender_from_domain, env_with_domain>())>);
    static_assert(requires {
        {
            test_std::get_completion_signatures<sender_with_get_completion_signatures, env_with_domain>()
        } -> std::same_as<signatures>;
    });

    //-dk:TODO do get_completion_signatures tests for awaitables
}

} // namespace

TEST(exec_getcomplsigs) {
    test_get_completion_signatures<int>();
    //-dk:TODO add actual tests
}
