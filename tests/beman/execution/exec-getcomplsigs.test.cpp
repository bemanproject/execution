// src/beman/execution/tests/exec-getcomplsigs.test.cpp             -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifdef BEMAN_USE_MODULE
import beman.execution;
#else
#include <beman/execution/execution.hpp>
#endif
#include <test/execution.hpp>
#include <concepts>

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

struct sender_with_get_completion_signatures {
    using sender_concept = test_std::sender_t;
    auto get_completion_signatures(const env&) const noexcept -> signatures { return {}; }
    auto get_completion_signatures(const other_env&) const noexcept -> other_signatures { return {}; }
};

struct domain {
    auto transform_sender(auto&&, auto&&...) const noexcept -> sender_with_typedef { return {}; }
};

struct env_with_domain {
    auto query(const test_std::get_domain_t&) const noexcept -> domain { return {}; }
};

template <typename T>
auto test_get_completion_signatures() {
    static_assert(test_std::sender<no_signatures_sender<T>>);
    static_assert(not requires { test_std::get_completion_signatures(no_signatures_sender<T>(), env{}); });

    static_assert(test_std::sender<sender_with_typedef>);
    static_assert(requires {
        { test_std::get_completion_signatures(sender_with_typedef(), env{}) } -> std::same_as<signatures>;
    });

    static_assert(test_std::sender<sender_with_get_completion_signatures>);
    static_assert(requires {
        { sender_with_get_completion_signatures().get_completion_signatures(env{}) } -> std::same_as<signatures>;
    });
    static_assert(requires {
        {
            test_std::get_completion_signatures(sender_with_get_completion_signatures(), env{})
        } -> std::same_as<signatures>;
    });
    static_assert(requires {
        {
            test_std::get_completion_signatures(sender_with_get_completion_signatures(), other_env{})
        } -> std::same_as<other_signatures>;
    });

    static_assert(std::same_as<domain, decltype(test_std::get_domain(env_with_domain{}))>);
    static_assert(
        std::same_as<sender_with_typedef,
                     decltype(test_std::transform_sender(domain{}, no_signatures_sender<int>{}, env_with_domain{}))>);
    static_assert(requires {
        {
            test_std::get_completion_signatures(sender_with_get_completion_signatures(), env_with_domain{})
        } -> std::same_as<signatures>;
    });

    //-dk:TODO do get_completion_signatures tests for awaitables
}
} // namespace

TEST(exec_getcomplsigs) {
    static_assert(
        std::same_as<const test_std::get_completion_signatures_t, decltype(test_std::get_completion_signatures)>);
    test_get_completion_signatures<int>();
    //-dk:TODO add actual tests
}
