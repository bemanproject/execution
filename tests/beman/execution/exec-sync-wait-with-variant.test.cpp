// src/beman/execution/tests/exec-sync-wait-with-variant.test.cpp                -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <exception>
#include <concepts>
#include <utility>
#include <tuple>
#include <variant>
#include <test/execution.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
import beman.execution.detail;
#else
#include <beman/execution/execution.hpp>
#endif

// ----------------------------------------------------------------------------

namespace {

template <typename... Values>
struct just_variant {
    using sender_concept        = test_std::sender_t;
    using completion_signatures = test_std::completion_signatures<test_std::set_value_t(Values)...>;

    template <typename, typename...>
    static consteval auto get_completion_signatures() -> completion_signatures {
        return {};
    }

    template <typename Receiver>
    struct state {
        using operation_state_concept = test_std::operation_state_t;

        auto start() & noexcept -> void {
            std::visit([this](auto v) { test_std::set_value(std::move(receiver), std::move(v)); }, std::move(var));
        }

        std::variant<Values...> var;
        Receiver                receiver;
    };

    template <typename Receiver>
    auto connect(Receiver receiver) && noexcept -> state<Receiver> {
        return {std::move(var), ::std::move(receiver)};
    }

    std::variant<Values...> var;
};

auto test_sync_wait_with_variant_multi_value() -> void {
    auto result1 = test_std::sync_wait_with_variant(just_variant<int, float>{114});
    static_assert(std::same_as<decltype(result1), std::optional<std::variant<std::tuple<int>, std::tuple<float>>>>);
    ASSERT(result1.has_value());
    ASSERT(result1.value().index() == 0);
    ASSERT(std::get<0>(*result1) == std::tuple{114});

    auto result2 = test_std::sync_wait_with_variant(just_variant<int, std::string>{"hello"});
    static_assert(
        std::same_as<decltype(result2), std::optional<std::variant<std::tuple<int>, std::tuple<std::string>>>>);
    ASSERT(result2.has_value());
    ASSERT(result2.value().index() == 1);
    ASSERT(std::get<1>(*result2) == std::tuple<std::string>{"hello"});
}

auto test_sync_wait_with_variant_single_value() -> void {
    auto value = test_std::sync_wait_with_variant(test_std::just(514));
    static_assert(std::same_as<decltype(value), std::optional<std::variant<std::tuple<int>>>>);
    ASSERT(value.has_value());
    ASSERT(std::get<0>(*value) == std::tuple{514});
}

} // namespace

TEST(exec_sync_wait_with_variant) {
    test_sync_wait_with_variant_multi_value();
    test_sync_wait_with_variant_single_value();
}
