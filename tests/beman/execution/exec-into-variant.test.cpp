// src/beman/execution/tests/exec-into-variant.test.cpp             -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <test/execution.hpp>
#include <concepts>
#include <exception>
#include <type_traits>
#include <variant>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution/detail/into_variant.hpp>
#include <beman/execution/detail/connect.hpp>
#include <beman/execution/detail/just.hpp>
#include <beman/execution/detail/receiver.hpp>
#include <beman/execution/detail/operation_state.hpp>
#include <beman/execution/detail/sender.hpp>
#endif

// ----------------------------------------------------------------------------

namespace {
struct non_sender {};
struct sender {
    using sender_concept = test_std::sender_t;
};
template <typename Signatures = test_std::completion_signatures<>, typename... T>
struct sender_in {
    using sender_concept        = test_std::sender_t;
    using completion_signatures = Signatures;
    decltype(test_std::just(std::declval<T>()...)) inner_sender;

    template <typename Receiver>
    auto connect(Receiver&& receiver) const noexcept {
        return test_std::connect(this->inner_sender, std::forward<Receiver>(receiver));
    }
};
template <typename... T>
sender_in(T&&...) -> sender_in<T...>;

struct receiver {
    using receiver_concept = test_std::receiver_t;

    bool* called{};

    auto set_error(auto&&) && noexcept -> void {}
    auto set_stopped() && noexcept -> void {}
    auto set_value(auto&&...) && noexcept -> void { *called = true; }
};

template <bool Expect, typename Signatures = void>
auto test_into_variant(auto&& sender) -> void {
    static_assert(Expect == test_std::sender<std::remove_cvref_t<decltype(sender)>>);
    static_assert(Expect == requires {
        { test_std::into_variant(sender) } -> test_std::sender;
    });
    if constexpr (Expect) {
        static_assert(test_std::receiver<receiver>);
        static_assert(requires {
            typename std::decay_t<decltype(sender)>;
            typename std::decay_t<decltype(sender)>::completion_signatures;
            test_std::get_completion_signatures(sender, test_std::env<>{});
            typename test_std::value_types_of_t<std::decay_t<decltype(sender)>, test_std::env<>>;
            test_std::into_variant(sender);
            test_std::connect(test_std::into_variant(sender), receiver{});
            { test_std::connect(test_std::into_variant(sender), receiver{}) } -> test_std::operation_state;
        });
        static_assert(std::same_as<Signatures,
                                   decltype(test_std::get_completion_signatures(test_std::into_variant(sender),
                                                                                test_std::env<>{}))>);
        bool called{false};
        auto op{test_std::connect(test_std::into_variant(sender), receiver{&called})};
        ASSERT(not called);
        test_std::start(op);
        ASSERT(called);
    }
}

struct error_receiver {
    using receiver_concept = test_std::receiver_t;

    bool* called{};

    auto set_error(int e) && noexcept -> void { *this->called = e == 17; }
    auto set_error(const std::exception_ptr&) && noexcept -> void {}
    auto set_stopped() && noexcept -> void {}
    auto set_value(auto&&...) && noexcept -> void {}
};

auto test_into_variant_error() -> void {
    bool called{};
    auto op{test_std::connect(test_std::into_variant(test_std::just_error(17)), error_receiver{&called})};
    ASSERT(not called);
    test_std::start(op);
    ASSERT(called);
}

struct stopped_receiver {
    using receiver_concept = test_std::receiver_t;

    bool* called{};

    auto set_error(const std::exception_ptr&) && noexcept -> void {}
    auto set_stopped() && noexcept -> void { *this->called = true; }
    auto set_value(auto&&...) && noexcept -> void {}
};

auto test_into_variant_stopped() -> void {
    bool called{};
    auto op{test_std::connect(test_std::into_variant(test_std::just_stopped()), stopped_receiver{&called})};
    ASSERT(not called);
    test_std::start(op);
    ASSERT(called);
}
} // namespace

TEST(exec_into_variant) {
    struct arg {};
    struct error {};

    static_assert(std::same_as<const test_std::into_variant_t, decltype(test_std::into_variant)>);

    test_into_variant_error();
    test_into_variant_stopped();

    test_into_variant<false>(non_sender{});
    // test_into_variant<true>(sender{});
    test_into_variant<true, test_std::completion_signatures<>>(sender_in<test_std::completion_signatures<>>{});

    test_into_variant<true, test_std::completion_signatures<test_std::set_value_t(std::variant<std::tuple<>>)>>(
        sender_in<test_std::completion_signatures<test_std::set_value_t()>>{});

    test_into_variant<true,
                      test_std::completion_signatures<test_std::set_value_t(std::variant<std::tuple<>>),
                                                      test_std::set_error_t(error)>>(
        sender_in<test_std::completion_signatures<test_std::set_value_t(), test_std::set_error_t(error)>>{});
    test_into_variant<true,
                      test_std::completion_signatures<test_std::set_value_t(std::variant<std::tuple<>>),
                                                      test_std::set_error_t(error),
                                                      test_std::set_stopped_t()>>(
        sender_in<test_std::completion_signatures<test_std::set_value_t(),
                                                  test_std::set_error_t(error),
                                                  test_std::set_stopped_t()>>{});
    test_into_variant<
        true,
        test_std::completion_signatures<test_std::set_value_t(std::variant<std::tuple<>, std::tuple<arg, arg>>),
                                        test_std::set_error_t(error),
                                        test_std::set_stopped_t()>>(
        sender_in<test_std::completion_signatures<test_std::set_value_t(),
                                                  test_std::set_value_t(arg, arg),
                                                  test_std::set_error_t(error),
                                                  test_std::set_stopped_t()>>{});
}
