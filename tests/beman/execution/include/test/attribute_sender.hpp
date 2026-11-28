// tests/beman/execution/include/test/attribute_sender.hpp            -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_TESTS_BEMAN_EXECUTION_INCLUDE_TEST_ATTRIBUTE_SENDER
#define INCLUDED_TESTS_BEMAN_EXECUTION_INCLUDE_TEST_ATTRIBUTE_SENDER

#include <test/execution.hpp>
#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <concepts>
#include <utility>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution.hpp>
#endif

// ----------------------------------------------------------------------------

namespace test {
struct value_domain : test_std::default_domain {};
struct error_domain : test_std::default_domain {};
struct stopped_domain : test_std::default_domain {};
using loop_scheduler = decltype(std::declval<test_std::run_loop&>().get_scheduler());

// A sender reporting a different completion scheduler and domain for each completion tag.
template <typename Value = int>
struct attribute_sender {
    using sender_concept        = test_std::sender_tag;
    using completion_signatures = test_std::
        completion_signatures<test_std::set_value_t(Value), test_std::set_error_t(int), test_std::set_stopped_t()>;
    template <typename, typename...>
    static consteval auto get_completion_signatures() -> completion_signatures {
        return {};
    }

    loop_scheduler value_scheduler;
    loop_scheduler error_scheduler;
    loop_scheduler stopped_scheduler;

    struct attributes {
        loop_scheduler value_scheduler;
        loop_scheduler error_scheduler;
        loop_scheduler stopped_scheduler;

        template <typename Tag, typename... Env>
        auto query(test_std::get_completion_scheduler_t<Tag>, const Env&...) const noexcept {
            if constexpr (std::same_as<Tag, test_std::set_value_t>)
                return value_scheduler;
            else if constexpr (std::same_as<Tag, test_std::set_error_t>)
                return error_scheduler;
            else
                return stopped_scheduler;
        }
        template <typename Tag, typename... Env>
            requires(!std::same_as<Tag, void>)
        static auto query(test_std::get_completion_domain_t<Tag>, const Env&...) noexcept {
            if constexpr (std::same_as<Tag, test_std::set_value_t>)
                return value_domain{};
            else if constexpr (std::same_as<Tag, test_std::set_error_t>)
                return error_domain{};
            else
                return stopped_domain{};
        }
    };
    auto get_env() const noexcept { return attributes{value_scheduler, error_scheduler, stopped_scheduler}; }
};

template <typename Tag, typename Sender, typename... Env>
concept has_completion_scheduler = requires(const Sender& sender, const Env&... env) {
    test_std::get_completion_scheduler<Tag>(test_std::get_env(sender), env...);
};

// The completion domain a sender reports for Tag in an empty environment.
template <typename Tag, typename Sender>
using completion_domain_t = decltype(test_std::get_completion_domain<Tag>(
    test_std::get_env(std::declval<const Sender&>()), test_std::env<>{}));
} // namespace test

// ----------------------------------------------------------------------------

#endif
