// src/beman/execution/tests/exec-when-all.test.cpp                 -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <test/execution.hpp>
#include <concepts>
#include <optional>
#include <utility>
#include <variant>
// #include <beman/execution/detail/suppress_push.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution/detail/when_all.hpp>
#include <beman/execution/detail/stop_callback_for_t.hpp>
#include <beman/execution/detail/when_all_with_variant.hpp>
#include <beman/execution/execution.hpp>
#endif

// ----------------------------------------------------------------------------

namespace {
template <typename... C>
struct multi_sender {
    using sender_concept        = test_std::sender_t;
    using completion_signatures = test_std::completion_signatures<C...>;
};

template <int>
struct domain {};

template <typename D>
struct domain_sender {
    struct env {
        auto query(test_std::get_domain_t) const noexcept -> D { return {}; }
    };
    using sender_concept        = test_std::sender_t;
    using completion_signatures = test_std::completion_signatures<test_std::set_value_t()>;
    auto get_env() const noexcept -> env { return {}; }
};

template <bool Expect, typename... S>
auto test_when_all_available(S&&... s) {
    static_assert(Expect == requires { test_std::when_all(std::forward<S>(s)...); });
}

auto test_when_all_breathing() -> void {
    auto s{test_std::when_all(test_std::just(), test_std::just(3, true), test_std::just(1.5))};
    static_assert(test_std::sender<decltype(s)>);
    test::check_type<test_std::completion_signatures<test_std::set_value_t(int, bool, double)>>(
        test_std::get_completion_signatures(s, test_std::env<>{}));
    auto res{test_std::sync_wait(s)};
    ASSERT(res.has_value());
    ASSERT((*res == std::tuple{3, true, 1.5}));
}

struct await_cancel {
    using sender_concept        = test_std::sender_t;
    using completion_signatures = test_std::completion_signatures<test_std::set_value_t(), test_std::set_stopped_t()>;

    template <typename Receiver>
    struct state {
        using operation_state_concept = test_std::operation_state_t;
        struct callback {
            Receiver* receiver;
            explicit callback(Receiver* rcvr) : receiver(rcvr) {}
            auto operator()() const noexcept -> void { test_std::set_stopped(std::move(*this->receiver)); }
        };

        using env           = decltype(test_std::get_env(std::declval<Receiver>()));
        using token         = test_std::stop_token_of_t<env>;
        using stop_callback = test_std::stop_callback_for_t<token, callback>;

        Receiver                     receiver;
        std::optional<stop_callback> cb;
        auto                         start() & noexcept {
            cb.emplace(test_std::get_stop_token(test_std::get_env(this->receiver)), &this->receiver);
        }
    };

    template <typename Receiver>
    auto connect(Receiver&& receiver) -> state<std::remove_cvref_t<Receiver>> {
        return {std::forward<Receiver>(receiver)};
    }
};

template <typename Sender, typename Tag, typename... T>
struct test_sender {
    using sender_concept        = test_std::sender_t;
    using completion_signatures = test_std::completion_signatures<test_std::set_value_t(bool)>;

    template <typename Result, typename Receiver>
    struct upstream {
        using receiver_concept = test_std::receiver_t;
        const Result& expect;
        Receiver&     receiver;
        auto          set_error(auto&& error) && noexcept -> void {
            if constexpr (std::same_as<Tag, test_std::set_error_t> && 1u == std::tuple_size_v<Result>)
                if constexpr (std::same_as<std::decay_t<std::tuple_element_t<0, Result>>,
                                                    std::decay_t<decltype(error)>>)
                    test_std::set_value(std::move(receiver), this->expect == std::tie(error));
                else
                    test_std::set_value(std::move(receiver), false);
            else
                test_std::set_value(std::move(receiver), false);
        }
        auto set_stopped() && noexcept -> void {
            test_std::set_value(std::move(receiver), std::same_as<Tag, test_std::set_stopped_t>);
        }
        auto set_value(auto&&... v) && noexcept -> void {
            if constexpr (std::same_as<Tag, test_std::set_value_t>)
                test_std::set_value(std::move(receiver), expect == std::tie(v...));
            else
                test_std::set_value(std::move(receiver), false);
        }
    };
    template <typename Result, typename Receiver>
    struct state {
        using operation_state_concept = test_std::operation_state_t;
        Result                                                                                          expect;
        std::remove_cvref_t<Receiver>                                                                   receiver;
        decltype(test_std::connect(std::declval<Sender>(), std::declval<upstream<Result, Receiver>>())) inner_state;
        template <typename S, typename R>
        state(S&& sender, auto&& exp, R&& rcvr)
            : expect(exp),
              receiver(std::forward<R>(rcvr)),
              inner_state(test_std::connect(std::forward<S>(sender),
                                            upstream<Result, Receiver>{this->expect, this->receiver})) {}
        auto start() & noexcept -> void { test_std::start(this->inner_state); }
    };

    Sender           sender;
    std::tuple<T...> expect;

    template <typename S>
    test_sender(S&& c, const std::tuple<Tag, T...>& value) : sender(std::forward<S>(c)) {
        std::apply([this](auto&&, auto&&... v) { this->expect = std::tie(v...); }, value);
    }
    template <typename Receiver>
    auto connect(Receiver&& receiver) && {
        return state<std::tuple<T...>, Receiver>(
            ::std::move(this->sender), this->expect, std::forward<Receiver>(receiver));
    }
};
template <typename Sender, typename Tag, typename... T>
test_sender(Sender, std::tuple<Tag, T...>) -> test_sender<std::remove_cvref_t<Sender>, Tag, T...>;

template <typename>
struct add_value_sig;
template <typename... S>
struct add_value_sig<test_std::completion_signatures<S...>> {
    using type = test_std::completion_signatures<S..., test_std::set_value_t(int)>;
};

template <typename Sender>
struct add_value {
    using sender_concept = test_std::sender_t;
    auto get_completion_signatures(const auto& env) const noexcept {
        using signatures = decltype(test_std::get_completion_signatures(sender, env));
        return typename add_value_sig<signatures>::type{};
    }
    Sender sender;
    template <typename Receiver>
    auto connect(Receiver&& receiver) && {
        return test_std::connect(std::move(this->sender), std::forward<Receiver>(receiver));
    }
    template <typename Receiver>
    auto connect(Receiver&& receiver) const& {
        return test_std::connect(std::move(this->sender), std::forward<Receiver>(receiver));
    }
};
template <typename Sender>
add_value(Sender) -> add_value<Sender>;

template <typename Result, test_std::sender... Sender>
auto test_when_all_results(Result&& res, Sender&&... sender) -> void {
    auto [result]{
        test_std::sync_wait(test_sender(test_std::when_all(std::forward<Sender>(sender)...), res)).value_or(false)};
    ASSERT(result);
}

auto test_when_all() -> void {
    test_when_all_available<false>();
    test_when_all_available<true>(test_std::just());
    test_when_all_available<true>(test_std::just(0), test_std::just('a', true), test_std::just(3.5));
    test_when_all_available<false>(test_std::just(), test_std::just(), int());
    test_when_all_available<true>(multi_sender<test_std::set_value_t(), test_std::set_stopped_t()>());
    test_when_all_available<true>(multi_sender<test_std::set_value_t(), test_std::set_error_t(int)>());
    test_when_all_available<false>(multi_sender<test_std::set_value_t(), test_std::set_value_t(int)>());
    test_when_all_available<false>(multi_sender<test_std::set_error_t(int)>());
    test_when_all_available<true>(domain_sender<domain<0>>(), domain_sender<domain<0>>());
    test_when_all_available<false>(domain_sender<domain<0>>(), domain_sender<domain<1>>());

    test_when_all_breathing();

    test_when_all_results(std::tuple{test_std::set_value, 'a', true, 3.5},
                          test_std::just(),
                          test_std::just('a'),
                          test_std::just(true, 3.5));
    test_when_all_results(std::tuple{test_std::set_value, 17}, test_std::just() | test_std::then([] { return 17; }));
    test_when_all_results(std::tuple{test_std::set_value, 17},
                          test_std::just() | test_std::then([]() noexcept { return 17; }));
    test_when_all_results(std::tuple{test_std::set_error, 17}, add_value{test_std::just_error(17)});
    test_when_all_results(std::tuple{test_std::set_error, 17},
                          test_std::just(),
                          add_value{test_std::just_error(17)},
                          test_std::just(true, 3.5));
    test_when_all_results(std::tuple{test_std::set_error, 17},
                          await_cancel(),
                          add_value{test_std::just_error(17)},
                          test_std::just(true, 3.5));
    test_when_all_results(std::tuple{test_std::set_stopped},
                          test_std::just(),
                          add_value{test_std::just_stopped()},
                          test_std::just(true, 3.5));
    test_when_all_results(std::tuple{test_std::set_stopped},
                          await_cancel(),
                          add_value{test_std::just_stopped()},
                          test_std::just(true, 3.5));
}

auto test_when_all_with_variant() -> void {
    auto s{test_std::when_all_with_variant(test_std::just(17), test_std::just('a', true))};
    auto res{test_std::sync_wait(s)};
    ASSERT(res);
    auto&& [v0, v1]{*res};
    test::use(v0, v1);
    std::visit(
        [](auto t) {
            auto [i]{t};
            ASSERT(i == 17);
        },
        v0);
    std::visit(
        [](auto t) {
            auto [c, b]{t};
            ASSERT(c == 'a');
            ASSERT(b == true);
        },
        v1);
}
} // namespace

// ----------------------------------------------------------------------------

TEST(exec_when_all) {
    static_assert(std::same_as<const test_std::when_all_t, decltype(test_std::when_all)>);
    static_assert(std::same_as<const test_std::when_all_with_variant_t, decltype(test_std::when_all_with_variant)>);

    try {

        test_when_all();
        test_when_all_with_variant();
    } catch (...) {
        // NOLINTNEXTLINE(cert-dcl03-c,hicpp-static-assert,misc-static-assert)
        ASSERT(nullptr == "the when_all tests shouldn't throw");
    }
}
