// src/beman/execution/tests/exec-utils-cmplsigs.test.cpp           -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/detail/sends_stopped.hpp>
#include <beman/execution/detail/error_types_of_t.hpp>
#include <beman/execution/detail/value_types_of_t.hpp>
#include <beman/execution/detail/gather_signatures.hpp>
#include <beman/execution/detail/indirect_meta_apply.hpp>
#include <beman/execution/detail/sender_in.hpp>
#include <beman/execution/execution.hpp>
#include <test/execution.hpp>
#include <concepts>
#include <exception>

// ----------------------------------------------------------------------------

namespace {
struct env {};
struct none_env {};
struct error {};
template <int>
struct arg {};

template <typename...>
struct type_list {};
template <typename...>
struct variant {};
template <typename...>
struct tuple {};
template <typename, typename>
struct two_types {};

struct sender {
    using sender_concept = test_std::sender_t;
    using empty_signatures =
        test_std::completion_signatures<test_std::set_error_t(error&&),
                                        test_std::set_error_t(const error&),
                                        test_std::set_value_t(),
                                        test_std::set_value_t(arg<0>, arg<1>&, arg<2>&&, const arg<3>&),
                                        test_std::set_stopped_t()>;
    auto get_completion_signatures(test_std::env<>) const noexcept { return empty_signatures(); }
    using env_signatures =
        test_std::completion_signatures<test_std::set_error_t(const error&),
                                        test_std::set_error_t(std::exception_ptr),
                                        test_std::set_value_t(arg<0>),
                                        test_std::set_value_t(arg<1>&),
                                        test_std::set_value_t(const arg<1>&),
                                        test_std::set_value_t(arg<0>, arg<1>&, arg<2>&&, const arg<3>&),
                                        test_std::set_stopped_t()>;
    auto get_completion_signatures(env) const noexcept { return env_signatures(); }
    using none_signatures = test_std::completion_signatures<>;
    auto get_completion_signatures(none_env) const noexcept { return none_signatures(); }
};

auto test_completion_signature() {
    static_assert(not test_detail::completion_signature<int>);
    static_assert(not test_detail::completion_signature<int()>);

    static_assert(not test_detail::completion_signature<test_std::set_error_t()>);
    static_assert(test_detail::completion_signature<test_std::set_error_t(error)>);
    static_assert(not test_detail::completion_signature<test_std::set_error_t(int, int)>);

    static_assert(test_detail::completion_signature<test_std::set_stopped_t()>);
    static_assert(not test_detail::completion_signature<test_std::set_stopped_t(int)>);

    static_assert(test_detail::completion_signature<test_std::set_value_t()>);
    static_assert(test_detail::completion_signature<test_std::set_value_t(arg<0>)>);
    static_assert(test_detail::completion_signature<test_std::set_value_t(arg<0>, arg<1>)>);
    static_assert(test_detail::completion_signature<test_std::set_value_t(arg<0>, arg<1>, arg<2>)>);
}

auto test_completion_signatures() -> void {
    test_std::completion_signatures<>();
    test_std::completion_signatures<test_std::set_error_t(error)>();
    //-dk:TODO create negative compilation test_std::completion_signatures<test_std::set_error_t()>();
    //-dk:TODO create negative compilation test_std::completion_signatures<test_std::set_error_t(error, error)>();

    test_std::completion_signatures<test_std::set_stopped_t()>();
    //-dk:TODO create negative compilation test_std::completion_signatures<test_std::set_stopped_t(int)>();

    test_std::completion_signatures<test_std::set_value_t()>();
    test_std::completion_signatures<test_std::set_value_t(arg<0>)>();
    test_std::completion_signatures<test_std::set_value_t(arg<0>, arg<1>)>();
    test_std::completion_signatures<test_std::set_value_t(arg<0>, arg<1>, arg<2>)>();

    test_std::completion_signatures<test_std::set_value_t(),
                                    test_std::set_value_t(arg<0>),
                                    test_std::set_value_t(arg<0>, arg<1>),
                                    test_std::set_value_t(arg<0>, arg<1>, arg<2>),
                                    test_std::set_error_t(error),
                                    test_std::set_error_t(error),
                                    test_std::set_stopped_t(),
                                    test_std::set_stopped_t()>();
}

auto test_valid_completion_signatures() -> void {
    static_assert(not test_detail::valid_completion_signatures<int>);
    static_assert(test_detail::valid_completion_signatures<test_std::completion_signatures<>>);
    static_assert(test_detail::valid_completion_signatures<test_std::completion_signatures<test_std::set_value_t()>>);
}

auto test_indirect_meta_apply() -> void {
    static_assert(std::same_as<type_list<bool, char, double>,
                               test_detail::indirect_meta_apply<true>::meta_apply<type_list, bool, char, double>>);

    static_assert(std::same_as<two_types<bool, char>,
                               test_detail::indirect_meta_apply<true>::meta_apply<two_types, bool, char>>);
}

auto test_always_true() -> void {
    static_assert(test_detail::always_true<>);
    static_assert(test_detail::always_true<arg<0>>);
    static_assert(test_detail::always_true<arg<0>, bool, char, double>);
}

auto test_gather_signatures() -> void {
    static_assert(
        std::same_as<variant<>,
                     test_detail::gather_signatures<test_std::set_error_t,
                                                    test_std::completion_signatures<test_std::set_stopped_t()>,
                                                    tuple,
                                                    variant>>);

    static_assert(
        std::same_as<variant<>,
                     test_detail::gather_signatures<test_std::set_error_t,
                                                    test_std::completion_signatures<test_std::set_stopped_t()>,
                                                    std::type_identity_t,
                                                    variant>>);

    static_assert(
        std::same_as<variant<tuple<>>,
                     test_detail::gather_signatures<test_std::set_stopped_t,
                                                    test_std::completion_signatures<test_std::set_stopped_t()>,
                                                    tuple,
                                                    variant>>);

    static_assert(
        std::same_as<tuple<>,
                     test_detail::gather_signatures<test_std::set_stopped_t,
                                                    test_std::completion_signatures<test_std::set_stopped_t()>,
                                                    tuple,
                                                    std::type_identity_t>>);

    static_assert(
        std::same_as<variant<tuple<int>, tuple<arg<0>>>,
                     test_detail::gather_signatures<test_std::set_error_t,
                                                    test_std::completion_signatures<test_std::set_value_t(),
                                                                                    test_std::set_value_t(int, arg<0>),
                                                                                    test_std::set_error_t(int),
                                                                                    test_std::set_error_t(arg<0>),
                                                                                    test_std::set_stopped_t()>,
                                                    tuple,
                                                    variant>>);

    static_assert(
        std::same_as<variant<int, arg<0>>,
                     test_detail::gather_signatures<test_std::set_error_t,
                                                    test_std::completion_signatures<test_std::set_value_t(),
                                                                                    test_std::set_value_t(int, arg<0>),
                                                                                    test_std::set_error_t(int),
                                                                                    test_std::set_error_t(arg<0>),
                                                                                    test_std::set_stopped_t()>,
                                                    std::type_identity_t,
                                                    variant>>);

    static_assert(
        std::same_as<
            variant<tuple<>, tuple<int, arg<0>, arg<1>>>,
            test_detail::gather_signatures<test_std::set_value_t,
                                           test_std::completion_signatures<test_std::set_value_t(),
                                                                           test_std::set_value_t(int, arg<0>, arg<1>),
                                                                           test_std::set_error_t(int),
                                                                           test_std::set_error_t(arg<0>),
                                                                           test_std::set_stopped_t()>,
                                           tuple,
                                           variant>>);
}

auto test_value_types_of_t() -> void {
    static_assert(test_std::sender_in<sender, test_std::env<>>);
    static_assert(
        std::same_as<sender::empty_signatures, test_std::completion_signatures_of_t<sender, test_std::env<>>>);
    static_assert(test_std::sender_in<sender, env>);
    static_assert(std::same_as<sender::env_signatures, test_std::completion_signatures_of_t<sender, env>>);
    static_assert(test_std::sender_in<sender, none_env>);
    static_assert(std::same_as<sender::none_signatures, test_std::completion_signatures_of_t<sender, none_env>>);

    static_assert(std::same_as<std::variant<std::tuple<>, std::tuple<arg<0>, arg<1>, arg<2>, arg<3>>>,
                               test_std::value_types_of_t<sender>>);
    static_assert(std::same_as<std::variant<std::tuple<>, std::tuple<arg<0>, arg<1>, arg<2>, arg<3>>>,
                               test_std::value_types_of_t<sender, test_std::env<>>>);
    static_assert(
        std::same_as<std::variant<std::tuple<arg<0>>, std::tuple<arg<1>>, std::tuple<arg<0>, arg<1>, arg<2>, arg<3>>>,
                     test_std::value_types_of_t<sender, env>>);
    static_assert(std::same_as<test_detail::empty_variant, test_std::value_types_of_t<sender, none_env>>);

    static_assert(std::same_as<variant<tuple<>, tuple<arg<0>, arg<1>&, arg<2>&&, const arg<3>&>>,
                               test_std::value_types_of_t<sender, test_std::env<>, tuple, variant>>);
}

auto test_error_types_of_t() -> void {
    static_assert(test_std::sender_in<sender, test_std::env<>>);
    static_assert(
        std::same_as<sender::empty_signatures, test_std::completion_signatures_of_t<sender, test_std::env<>>>);
    static_assert(test_std::sender_in<sender, env>);
    static_assert(std::same_as<sender::env_signatures, test_std::completion_signatures_of_t<sender, env>>);
    static_assert(test_std::sender_in<sender, none_env>);
    static_assert(std::same_as<sender::none_signatures, test_std::completion_signatures_of_t<sender, none_env>>);

    static_assert(std::same_as<std::variant<error>, test_std::error_types_of_t<sender>>);
    static_assert(std::same_as<std::variant<error>, test_std::error_types_of_t<sender, test_std::env<>>>);
    static_assert(std::same_as<std::variant<error, std::exception_ptr>, test_std::error_types_of_t<sender, env>>);
    static_assert(std::same_as<test_detail::empty_variant, test_std::error_types_of_t<sender, none_env>>);
    static_assert(
        std::same_as<variant<const error&, std::exception_ptr>, test_std::error_types_of_t<sender, env, variant>>);
}

auto test_sends_stopped() -> void {
    static_assert(test_std::sender_in<sender, test_std::env<>>);
    static_assert(
        std::same_as<sender::empty_signatures, test_std::completion_signatures_of_t<sender, test_std::env<>>>);
    static_assert(test_std::sender_in<sender, env>);
    static_assert(std::same_as<sender::env_signatures, test_std::completion_signatures_of_t<sender, env>>);
    static_assert(test_std::sender_in<sender, none_env>);
    static_assert(std::same_as<sender::none_signatures, test_std::completion_signatures_of_t<sender, none_env>>);

    static_assert(test_std::sends_stopped<sender>);
    static_assert(test_std::sends_stopped<sender, test_std::env<>>);
    static_assert(test_std::sends_stopped<sender, env>);
    static_assert(not test_std::sends_stopped<sender, none_env>);
}
} // namespace

TEST(exec_utils_cmplsigs) {
    test_completion_signature();
    test_completion_signatures();
    test_valid_completion_signatures();
    test_indirect_meta_apply();
    //-dk:TODO add other components mentioned by [exec.utils.cmplsigs]:
    test_always_true();
    test_gather_signatures();
    test_value_types_of_t();
    test_error_types_of_t();
    test_sends_stopped();
}
