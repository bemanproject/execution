// src/beman/execution/tests/execution-syn.test.cpp                 -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/detail/sender_adaptor.hpp>
#include <beman/execution/detail/sender_adaptor_closure.hpp>
#include <beman/execution/detail/decays_to.hpp>
#include <beman/execution/detail/connect_result_t.hpp>
#include <beman/execution/detail/connect.hpp>
#include <beman/execution/detail/single_sender.hpp>
#include <beman/execution/detail/single_sender_value_type.hpp>
#include <beman/execution/detail/type_list.hpp>
#include <beman/execution/detail/schedule_result_t.hpp>
#include <beman/execution/detail/env_of_t.hpp>
#include <beman/execution/detail/decayed_tuple.hpp>
#include <beman/execution/detail/variant_or_empty.hpp>
#include <beman/execution/detail/completion_signatures_of_t.hpp>
#include <beman/execution/detail/as_awaitable.hpp>
#include <beman/execution/execution.hpp>
#include <test/execution.hpp>
#include <concepts>

#include <beman/execution/detail/suppress_push.hpp>

// ----------------------------------------------------------------------------

namespace {
auto use(auto&&) -> void {}
struct scheduler {
    struct env {
        template <test_detail::completion_tag Tag>
        auto query(const test_std::get_completion_scheduler_t<Tag>&) const noexcept -> scheduler {
            return {};
        }
    };
    struct sender {
        using sender_concept = test_std::sender_t;
        auto get_env() const noexcept -> env { return {}; }
    };
    using scheduler_concept = test_std::scheduler_t;

    auto schedule() const noexcept -> sender { return {}; }
    auto operator==(const scheduler&) const -> bool = default;
};

struct test_env {};
struct no_value_env {};
struct single_type_sender {
    struct arg {};
    struct error {};
    using sender_concept  = test_std::sender_t;
    using test_signatures = test_std::completion_signatures<test_std::set_error_t(error),
                                                            test_std::set_error_t(int),
                                                            test_std::set_value_t(arg&),
                                                            test_std::set_stopped_t()>;
    auto get_completion_signatures(const test_env&) const noexcept { return test_signatures(); }
    using empty_signatures = test_std::completion_signatures<test_std::set_error_t(error),
                                                             test_std::set_error_t(int),
                                                             test_std::set_value_t(bool&),
                                                             test_std::set_stopped_t()>;
    auto get_completion_signatures(const test_std::env<>&) const noexcept { return empty_signatures(); }
    using no_value_signatures = test_std::
        completion_signatures<test_std::set_error_t(error), test_std::set_error_t(int), test_std::set_stopped_t()>;
    auto get_completion_signatures(const no_value_env&) const noexcept { return no_value_signatures(); }
};

struct void_sender {
    struct error {};
    using sender_concept        = test_std::sender_t;
    using completion_signatures = test_std::completion_signatures<test_std::set_error_t(error),
                                                                  test_std::set_error_t(int),
                                                                  test_std::set_value_t(),
                                                                  test_std::set_stopped_t()>;
};

struct no_value_sender {
    struct error {};
    using sender_concept        = test_std::sender_t;
    using completion_signatures = test_std::
        completion_signatures<test_std::set_error_t(error), test_std::set_error_t(int), test_std::set_stopped_t()>;
};

struct multi_single_sender {
    struct arg {};
    struct error {};
    using sender_concept        = test_std::sender_t;
    using completion_signatures = test_std::completion_signatures<test_std::set_error_t(error),
                                                                  test_std::set_error_t(int),
                                                                  test_std::set_value_t(arg&),
                                                                  test_std::set_value_t(int),
                                                                  test_std::set_stopped_t()>;
};

struct multi_type_sender {
    struct arg {};
    struct error {};
    using sender_concept        = test_std::sender_t;
    using completion_signatures = test_std::completion_signatures<test_std::set_error_t(error),
                                                                  test_std::set_error_t(int),
                                                                  test_std::set_value_t(arg&, const bool&, int),
                                                                  test_std::set_stopped_t()>;
};

auto test_schedule_result_t() -> void {
    static_assert(test_std::sender<scheduler::sender>);
    static_assert(test_std::scheduler<scheduler>);
    static_assert(std::same_as<scheduler::sender, test_std::schedule_result_t<scheduler>>);
}

auto test_env_of_t() -> void {
    struct env {};
    struct object {};
    struct object_with_env {
        auto get_env() const noexcept -> env { return {}; }
    };

    static_assert(std::same_as<test_std::env<>, test_std::env_of_t<object>>);
    static_assert(std::same_as<env, test_std::env_of_t<object_with_env>>);
}

auto test_decayed_tuple() -> void {
    static_assert(std::same_as<std::tuple<int, bool, char>, test_detail::decayed_tuple<int, bool, char>>);
    static_assert(std::same_as<std::tuple<int, bool, char>,
                               test_detail::decayed_tuple<const int, volatile bool, const volatile char>>);
    static_assert(std::same_as<std::tuple<int, bool, char*>,
                               test_detail::decayed_tuple<int&, bool&&, char[5]>>); // NOLINT(hicpp-avoid-c-arrays)
    static_assert(std::same_as<
                  std::tuple<int, bool, char*>,
                  test_detail::decayed_tuple<const int&, volatile bool&&, char[5]>>); // NOLINT(hicpp-avoid-c-arrays)
}

auto test_variant_or_empty() -> void {
    static_assert(not std::default_initializable<test_detail::empty_variant>);

    static_assert(std::same_as<test_detail::variant_or_empty<>, test_detail::empty_variant>);
    static_assert(std::same_as<test_detail::variant_or_empty<int&>, std::variant<int>>);
    static_assert(std::same_as<test_detail::variant_or_empty<int&, const int>, std::variant<int>>);
}

struct sender_in {
    struct arg {};

    using sender_concept = test_std::sender_t;
    using completion_signatures =
        test_std::completion_signatures<test_std::set_value_t(arg), test_std::set_stopped_t()>;
};

struct env {};
struct sender_with_get {
    struct arg {};

    using sender_concept = test_std::sender_t;

    using empty_sigs = test_std::completion_signatures<test_std::set_value_t(arg), test_std::set_stopped_t()>;
    auto get_completion_signatures(test_std::env<>) const noexcept { return empty_sigs{}; }

    using env_sigs = test_std::completion_signatures<test_std::set_value_t(arg, arg), test_std::set_stopped_t()>;
    auto get_completion_signatures(env) const noexcept { return env_sigs{}; }
};

template <typename T>
auto test_completion_signatures_of_t() -> void {
    struct non_sender {};
    static_assert(not requires {
        typename T;
        typename test_std::completion_signatures_of_t<non_sender>;
    });

    static_assert(test_std::sender<sender_in>);
    static_assert(test_std::sender_in<sender_in>);
    static_assert(requires {
        typename test_std::completion_signatures_of_t<sender_in>;
        {
            test_std::completion_signatures_of_t<sender_in> {}
        } -> std::same_as<sender_in::completion_signatures>;
    });

    static_assert(not std::same_as<typename sender_with_get::empty_sigs, typename sender_with_get::env_sigs>);
    static_assert(test_std::sender<sender_with_get>);
    static_assert(test_std::sender_in<sender_with_get>);
    static_assert(requires {
        typename test_std::completion_signatures_of_t<sender_with_get>;
        {
            test_std::completion_signatures_of_t<sender_with_get> {}
        } -> std::same_as<typename sender_with_get::empty_sigs>;
    });
    static_assert(requires {
        typename test_std::completion_signatures_of_t<sender_with_get>;
        {
            test_std::completion_signatures_of_t<sender_with_get, env> {}
        } -> std::same_as<typename sender_with_get::env_sigs>;
    });
}

auto test_type_list() -> void {
    struct arg {};
    static_assert(std::same_as<test_detail::type_list<>, test_detail::type_list<>>);
    static_assert(std::same_as<test_detail::type_list<arg, bool, char&, const double&, long&&>,
                               test_detail::type_list<arg, bool, char&, const double&, long&&>>);
}

template <bool Expect, typename Sender, typename Env>
auto test_single_sender_value_type(Sender, Env) -> void {
    static_assert(test_std::sender_in<Sender>);
    static_assert(Expect == requires { typename test_detail::single_sender_value_type<Sender, Env>; });
}

auto test_single_sender_value_type() -> void {
    static_assert(test_std::sender<single_type_sender>);
    static_assert(test_std::sender<void_sender>);
    static_assert(test_std::sender<multi_single_sender>);
    static_assert(test_std::sender<multi_type_sender>);
    static_assert(test_std::sender<no_value_sender>);

    static_assert(requires { typename test_detail::single_sender_value_type<single_type_sender, test_env>; });
    static_assert(
        std::same_as<single_type_sender::arg, test_detail::single_sender_value_type<single_type_sender, test_env>>);
    static_assert(requires { typename test_detail::single_sender_value_type<single_type_sender, test_std::env<>>; });
    static_assert(std::same_as<bool, test_detail::single_sender_value_type<single_type_sender, test_std::env<>>>);
    static_assert(requires { typename test_detail::single_sender_value_type<void_sender, test_std::env<>>; });
    static_assert(std::same_as<void, test_detail::single_sender_value_type<void_sender, test_std::env<>>>);
    static_assert(requires { typename test_detail::single_sender_value_type<multi_type_sender, test_std::env<>>; });
    static_assert(std::same_as<std::tuple<multi_type_sender::arg, bool, int>,
                               test_detail::single_sender_value_type<multi_type_sender, test_std::env<>>>);
    test_single_sender_value_type<true>(single_type_sender{}, test_std::env<>{});
    test_single_sender_value_type<true>(single_type_sender{}, test_env{});
    test_single_sender_value_type<true>(single_type_sender{}, no_value_env{});
    test_single_sender_value_type<false>(multi_single_sender{}, test_std::env<>{});
    test_single_sender_value_type<true>(no_value_sender{}, test_std::env<>{});
}

auto test_single_sender() -> void {
    static_assert(test_detail::single_sender<single_type_sender, test_std::env<>>);
    static_assert(test_detail::single_sender<single_type_sender, no_value_env>);
    static_assert(test_detail::single_sender<void_sender, test_std::env<>>);
    static_assert(not test_detail::single_sender<multi_single_sender, test_std::env<>>);
    static_assert(test_detail::single_sender<multi_type_sender, test_std::env<>>);
    static_assert(test_detail::single_sender<no_value_sender, test_std::env<>>);
}

struct connect_sender {
    using sender_concept = test_std::sender_t;
    template <typename Receiver>
    struct state {
        using operation_state_concept = test_std::operation_state_t;
        auto start() noexcept -> void {}
    };
    struct tag {};

    tag t{};
    int d{};

    template <test_std::receiver Receiver>
    auto connect(Receiver&&) const noexcept -> state<Receiver> {
        return {};
    }
};

auto test_conect_result_t() -> void {
    struct receiver {
        using receiver_concept = test_std::receiver_t;
    };

    static_assert(test_std::sender<connect_sender>);
    static_assert(test_std::receiver<receiver>);
    static_assert(requires { connect_sender{}.connect(receiver{}); });

    auto op{test_std::connect(connect_sender{}, receiver{})};
    test::use(op);
    static_assert(std::same_as<decltype(op), test_std::connect_result_t<connect_sender, receiver>>);
}
auto test_decays_to() -> void {
    struct type {};
    struct other {};

    static_assert(test_detail::decays_to<type, type>);
    static_assert(test_detail::decays_to<type&, type>);
    static_assert(test_detail::decays_to<type&&, type>);
    static_assert(test_detail::decays_to<const type&, type>);
    static_assert(test_detail::decays_to<type[1], type*>); // NOLINT(hicpp-avoid-c-arrays)
    static_assert(test_detail::decays_to<type[], type*>);  // NOLINT(hicpp-avoid-c-arrays)
    static_assert(not test_detail::decays_to<type&, type&>);
    static_assert(not test_detail::decays_to<type, type&>);
    static_assert(not test_detail::decays_to<other, type>);
}

template <test_std::sender>
struct adapted_sender {
    using sender_concept = test_std::sender_t;
};

struct closure_t : test_std::sender_adaptor_closure<closure_t> {
    template <test_std::sender Sender>
    auto operator()(Sender&&) const -> adapted_sender<Sender> {
        return {};
    }
};
constexpr closure_t closure{};

auto test_sender_adaptor_closure() -> void {
    use(test_std::sender_adaptor_closure<int>{});
    struct sender {
        using sender_concept = test_std::sender_t;
    };
    static_assert(test_std::sender<sender>);

    static_assert(test_std::sender<adapted_sender<sender>>);
    static_assert(not test_std::sender<closure_t>);

    auto direct{closure(sender{})};
    test::use(direct);
    static_assert(std::same_as<adapted_sender<sender>, decltype(direct)>);
    auto via_op{sender{} | closure};
    test::use(via_op);
    static_assert(std::same_as<adapted_sender<sender>, decltype(via_op)>);
}

struct arg_closure_t {
    template <test_std::sender Sender>
    auto operator()(Sender&&, int) const -> adapted_sender<Sender> {
        return {};
    }
    auto operator()(int value) const { return test_detail::sender_adaptor<arg_closure_t, int>{*this, value, {}}; }
};
constexpr arg_closure_t arg_closure{};

auto test_sender_adaptor() -> void {
    struct sender {
        using sender_concept = test_std::sender_t;
    };
    static_assert(test_std::sender<sender>);

    auto closure_{arg_closure(17)};
    static_assert(std::same_as<test_detail::sender_adaptor<arg_closure_t, int>, decltype(closure_)>);
    auto direct{closure_(sender{})};
    test::use(direct);
    static_assert(std::same_as<adapted_sender<sender>, decltype(direct)>);
    auto via_op{sender{} | closure_};
    test::use(via_op);
    static_assert(std::same_as<adapted_sender<sender>, decltype(via_op)>);
}

auto test_as_awaitable() -> void {
    static_assert(std::same_as<const test_std::as_awaitable_t, decltype(test_std::as_awaitable)>);
}
} // namespace

TEST(execution_syn) {
    test_schedule_result_t();
    test_env_of_t();
    test_decayed_tuple();
    test_variant_or_empty();
    test_completion_signatures_of_t<int>();
    test_type_list();
    test_single_sender_value_type();
    test_single_sender();
    test_conect_result_t();
    test_decays_to();
    test_sender_adaptor_closure();
    test_sender_adaptor();
    test_as_awaitable();
}
