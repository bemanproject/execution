// tests/beman/execution/exec-spawn-future.test.cpp                   -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/detail/spawn_future.hpp>
#include <beman/execution/detail/queryable.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/async_scope_token.hpp>
#include <test/execution.hpp>
#include <concepts>

// ----------------------------------------------------------------------------

namespace
{
    struct non_env { ~non_env() = delete; };
    static_assert(not test_detail::queryable<non_env>);

    struct env {};
    static_assert(test_detail::queryable<env>);

    struct non_sender {
    };
    static_assert(not test_std::sender<non_sender>);

    struct sender {
        using sender_concept = test_std::sender_t;
    };
    static_assert(test_std::sender<sender>);

    template <bool Noexcept>
    struct token {
        auto try_associate() -> bool { return {}; }
        auto disassociate() noexcept(Noexcept) -> void {}
        template <test_std::sender Sender>
        auto wrap(Sender&& sender) -> Sender { return std::forward<Sender>(sender); }
    };
    static_assert(test_std::async_scope_token<token<true>>);
    static_assert(not test_std::async_scope_token<token<false>>);

    struct throws {
        throws() = default;
        throws(throws&&) noexcept(false) = default;
    };
    static_assert(!std::is_nothrow_constructible_v<std::decay_t<throws>, throws>);

    template <bool Expect, typename Sender, typename Token, typename Env = ::env>
    auto test_spawn_future_interface(Sender&& sndr, Token&& tok, Env&& e = Env{}) -> void {
        if constexpr (!std::same_as<std::remove_cvref_t<Env>, non_env>){
            static_assert(Expect == requires{ test_std::spawn_future(std::forward<Sender>(sndr), std::forward<Token>(tok)); });
        }
        static_assert(Expect == requires{ test_std::spawn_future(std::forward<Sender>(sndr), std::forward<Token>(tok), std::forward<Env>(e)); });
    }

    template <typename Completions>
    struct state_base: test_detail::spawn_future_state_base<Completions> {
        auto complete() noexcept -> void override {}
    };
    auto test_state_base() {
        static_assert(noexcept(std::declval<test_detail::spawn_future_state_base<test_std::completion_signatures<>>&>().complete()));
        using state0_t = state_base<test_std::completion_signatures<>>;
        [[maybe_unused]] state0_t b0;
        static_assert(std::same_as<std::variant<std::monostate>, state0_t::variant_t>);
        static_assert(std::same_as<state0_t::variant_t, decltype(b0.result)>);

        using state1_t = state_base<test_std::completion_signatures<test_std::set_value_t(int)>>;
        [[maybe_unused]] state1_t b1;
        static_assert(std::same_as<std::variant<std::monostate, std::tuple<test_std::set_value_t, int>>, state1_t::variant_t>);
        static_assert(std::same_as<state1_t::variant_t, decltype(b1.result)>);

        using state2_t = state_base<test_std::completion_signatures<test_std::set_value_t(throws)>>;
        [[maybe_unused]] state2_t b2;
        static_assert(std::same_as<std::variant<std::monostate, std::tuple<test_std::set_error_t, std::exception_ptr>, std::tuple<test_std::set_value_t, throws>>, typename state2_t::variant_t>);
        static_assert(std::same_as<state2_t::variant_t, decltype(b2.result)>);

        using state3_t = state_base<test_std::completion_signatures<test_std::set_value_t(throws), test_std::set_error_t(std::exception_ptr)>>;
        [[maybe_unused]] state3_t b3;
        static_assert(std::same_as<std::variant<std::monostate, std::tuple<test_std::set_error_t, std::exception_ptr>, std::tuple<test_std::set_value_t, throws>>, typename state3_t::variant_t>);
        static_assert(std::same_as<state3_t::variant_t, decltype(b3.result)>);

        using state4_t = state_base<test_std::completion_signatures<test_std::set_value_t(throws), test_std::set_value_t(throws const&)>>;
        [[maybe_unused]] state4_t b4;
        static_assert(std::same_as<std::variant<std::monostate, std::tuple<test_std::set_error_t, std::exception_ptr>, std::tuple<test_std::set_value_t, throws>>, typename state4_t::variant_t>);
        static_assert(std::same_as<state4_t::variant_t, decltype(b4.result)>);
    }
}

TEST(exec_spawn_future) {
    static_assert(std::same_as<test_std::spawn_future_t const, decltype(test_std::spawn_future)>);
    test_spawn_future_interface<true>(sender{}, token<true>{});
    test_spawn_future_interface<false>(non_sender{}, token<true>{});
    test_spawn_future_interface<false>(sender{}, token<false>{});
    test_spawn_future_interface<false>(sender{}, token<true>{}, *new non_env{});

    test_state_base();
}
