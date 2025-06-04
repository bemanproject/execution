// tests/beman/execution/exec-spawn-future.test.cpp                   -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/detail/spawn_future.hpp>
#include <beman/execution/detail/queryable.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/async_scope_token.hpp>
#include <beman/execution/detail/receiver.hpp>
#include <beman/execution/detail/simple_allocator.hpp>
#include <beman/execution/detail/get_allocator.hpp>
#include <beman/execution/detail/get_stop_token.hpp>
#include <beman/execution/detail/join_env.hpp>
#include <beman/execution/detail/inplace_stop_source.hpp>
#include <test/execution.hpp>
#include <concepts>

// ----------------------------------------------------------------------------

namespace {
struct non_env {
    ~non_env() = delete;
};
static_assert(not test_detail::queryable<non_env>);

struct env {
    int  value{};
    auto operator==(const env&) const -> bool = default;
};
static_assert(test_detail::queryable<env>);

struct non_sender {};
static_assert(not test_std::sender<non_sender>);

struct sender {
    using sender_concept        = test_std::sender_t;
    using completion_signatures = test_std::completion_signatures<test_std::set_value_t()>;
};
static_assert(test_std::sender<sender>);

template <bool Noexcept>
struct token {
    auto try_associate() -> bool { return {}; }
    auto disassociate() noexcept(Noexcept) -> void {}
    template <test_std::sender Sender>
    auto wrap(Sender&& sender) -> Sender {
        return std::forward<Sender>(sender);
    }
};
static_assert(test_std::async_scope_token<token<true>>);
static_assert(not test_std::async_scope_token<token<false>>);

struct exception {
    int value;
};
struct throws {
    throws() = default;
    throws(throws&&) noexcept(false) { throw exception{42}; }
};
static_assert(!std::is_nothrow_constructible_v<std::decay_t<throws>, throws>);

template <bool Expect, typename Env = ::env, typename Sender, typename Token>
auto test_spawn_future_interface(Sender&& sndr, Token&& tok) -> void {
    if constexpr (!std::same_as<std::remove_cvref_t<Env>, non_env>) {
        static_assert(Expect ==
                      requires { test_std::spawn_future(std::forward<Sender>(sndr), std::forward<Token>(tok)); });
    }
    static_assert(Expect == requires(const Env& e) {
        test_std::spawn_future(std::forward<Sender>(sndr), std::forward<Token>(tok), e);
    });
}

template <typename Completions>
struct state_base : test_detail::spawn_future_state_base<Completions> {
    bool called{false};
    auto complete() noexcept -> void override { this->called = true; }
};
auto test_state_base() {
    static_assert(
        noexcept(std::declval<test_detail::spawn_future_state_base<test_std::completion_signatures<>>&>().complete()));
    using state0_t = state_base<test_std::completion_signatures<>>;
    [[maybe_unused]] state0_t b0;
    static_assert(std::same_as<std::variant<std::monostate>, state0_t::variant_t>);
    static_assert(std::same_as<state0_t::variant_t, decltype(b0.result)>);

    using state1_t = state_base<test_std::completion_signatures<test_std::set_value_t(int)>>;
    [[maybe_unused]] state1_t b1;
    static_assert(
        std::same_as<std::variant<std::monostate, std::tuple<test_std::set_value_t, int>>, state1_t::variant_t>);
    static_assert(std::same_as<state1_t::variant_t, decltype(b1.result)>);

    using state2_t = state_base<test_std::completion_signatures<test_std::set_value_t(throws)>>;
    [[maybe_unused]] state2_t b2;
    static_assert(std::same_as<std::variant<std::monostate,
                                            std::tuple<test_std::set_error_t, std::exception_ptr>,
                                            std::tuple<test_std::set_value_t, throws>>,
                               typename state2_t::variant_t>);
    static_assert(std::same_as<state2_t::variant_t, decltype(b2.result)>);

    using state3_t = state_base<
        test_std::completion_signatures<test_std::set_value_t(throws), test_std::set_error_t(std::exception_ptr)>>;
    [[maybe_unused]] state3_t b3;
    static_assert(std::same_as<std::variant<std::monostate,
                                            std::tuple<test_std::set_error_t, std::exception_ptr>,
                                            std::tuple<test_std::set_value_t, throws>>,
                               typename state3_t::variant_t>);
    static_assert(std::same_as<state3_t::variant_t, decltype(b3.result)>);

    using state4_t = state_base<
        test_std::completion_signatures<test_std::set_value_t(throws), test_std::set_value_t(const throws&)>>;
    [[maybe_unused]] state4_t b4;
    static_assert(std::same_as<std::variant<std::monostate,
                                            std::tuple<test_std::set_error_t, std::exception_ptr>,
                                            std::tuple<test_std::set_value_t, throws>>,
                               typename state4_t::variant_t>);
    static_assert(std::same_as<state4_t::variant_t, decltype(b4.result)>);

    using state5_t = state_base<test_std::completion_signatures<test_std::set_stopped_t()>>;
    [[maybe_unused]] state5_t b5;
    static_assert(
        std::same_as<std::variant<std::monostate, std::tuple<test_std::set_stopped_t>>, typename state5_t::variant_t>);
    static_assert(std::same_as<state5_t::variant_t, decltype(b5.result)>);
}

auto test_receiver() {
    {
        using c_t = test_std::completion_signatures<test_std::set_stopped_t()>;
        state_base<c_t> state{};
        ASSERT(state.called == false);
        ASSERT(std::holds_alternative<std::monostate>(state.result));
        static_assert(test_std::receiver<test_detail::spawn_future_receiver<c_t>>);
        [[maybe_unused]] test_detail::spawn_future_receiver<c_t> r0{&state};
        [](auto&& r) { static_assert(not requires { r.set_stopped(); }); }(r0);
        static_assert(noexcept(std::move(r0).set_stopped()));
        std::move(r0).set_stopped();
        ASSERT(state.called == true);
        ASSERT((std::holds_alternative<std::tuple<test_std::set_stopped_t>>(state.result)));
    }

    {
        using c_t = test_std::completion_signatures<test_std::set_error_t(int)>;
        state_base<c_t> state{};
        ASSERT(state.called == false);
        ASSERT(std::holds_alternative<std::monostate>(state.result));
        static_assert(test_std::receiver<test_detail::spawn_future_receiver<c_t>>);
        [[maybe_unused]] test_detail::spawn_future_receiver<c_t> r0{&state};
        [](auto&& r) { static_assert(not requires { r.set_error(17); }); }(r0);
        static_assert(noexcept(std::move(r0).set_error(17)));
        std::move(r0).set_error(17);
        ASSERT(state.called == true);
        ASSERT((std::holds_alternative<std::tuple<test_std::set_error_t, int>>(state.result)));
        ASSERT((std::get<1>(std::get<std::tuple<test_std::set_error_t, int>>(state.result)) == 17));
    }

    {
        using c_t = test_std::completion_signatures<test_std::set_value_t(int, bool, char)>;
        state_base<c_t> state{};
        ASSERT(state.called == false);
        ASSERT(std::holds_alternative<std::monostate>(state.result));
        static_assert(test_std::receiver<test_detail::spawn_future_receiver<c_t>>);
        [[maybe_unused]] test_detail::spawn_future_receiver<c_t> r0{&state};
        [](auto&& r) { static_assert(not requires { r.set_value(17, true, 'x'); }); }(r0);
        static_assert(noexcept(std::move(r0).set_value(17, true, 'x')));
        std::move(r0).set_value(17, true, 'x');
        ASSERT(state.called == true);
        ASSERT((std::holds_alternative<std::tuple<test_std::set_value_t, int, bool, char>>(state.result)));
        ASSERT((std::get<1>(std::get<std::tuple<test_std::set_value_t, int, bool, char>>(state.result)) == 17));
        ASSERT((std::get<2>(std::get<std::tuple<test_std::set_value_t, int, bool, char>>(state.result)) == true));
        ASSERT((std::get<3>(std::get<std::tuple<test_std::set_value_t, int, bool, char>>(state.result)) == 'x'));
    }

    {
        using c_t = test_std::completion_signatures<test_std::set_value_t(int, throws, char)>;
        state_base<c_t> state{};
        ASSERT(state.called == false);
        ASSERT(std::holds_alternative<std::monostate>(state.result));
        static_assert(test_std::receiver<test_detail::spawn_future_receiver<c_t>>);
        [[maybe_unused]] test_detail::spawn_future_receiver<c_t> r0{&state};
        [](auto&& r) { static_assert(not requires { r.set_value(17, throws(), 'x'); }); }(r0);
        static_assert(noexcept(std::move(r0).set_value(17, throws(), 'x')));
        std::move(r0).set_value(17, throws(), 'x');
        ASSERT(state.called == true);
        ASSERT((std::holds_alternative<std::tuple<test_std::set_error_t, std::exception_ptr>>(state.result)));
        try {
            std::rethrow_exception(
                std::get<1>(std::get<std::tuple<test_std::set_error_t, std::exception_ptr>>(state.result)));
            ASSERT(nullptr == "not reached");
        } catch (const exception& ex) {
            ASSERT(ex.value == 42);
        } catch (...) {
            ASSERT(nullptr == "not reached");
        }
    }
}

template <test_std::sender Sndr, test_std::async_scope_token Tok, typename Ev>
auto test_spawn_future(Sndr&& sndr, Tok&& tok, Ev&& ev) {
    test_std::spawn_future(std::forward<Sndr>(sndr), std::forward<Tok>(tok), std::forward<Ev>(ev));
}

struct allocator {
    using value_type = int;
    int value{};

    auto allocate(std::size_t n) -> int* { return new int[n]; }
    auto deallocate(int* p, std::size_t) -> void { delete[] p; }

    auto operator==(const allocator&) const -> bool = default;
};
static_assert(test_detail::simple_allocator<allocator>);

struct alloc_env {
    int  value{};
    auto operator==(const alloc_env&) const -> bool = default;

    auto query(const test_std::get_allocator_t&) const noexcept -> allocator { return allocator{this->value}; }
};

struct alloc_sender {
    using sender_concept = test_std::sender_t;
    int value{};

    auto get_env() const noexcept -> alloc_env { return alloc_env{this->value}; }
};
static_assert(test_std::sender<alloc_sender>);

auto test_get_allocator() {
    {
        alloc_env ae{87};
        auto [alloc, ev] = test_detail::spawn_future_get_allocator(sender{}, ae);
        static_assert(std::same_as<decltype(alloc), allocator>);
        ASSERT(alloc == allocator{87});
        static_assert(std::same_as<decltype(ev), alloc_env>);
        ASSERT(ev == alloc_env{87});
    }
    {
        auto [alloc, ev] = test_detail::spawn_future_get_allocator(alloc_sender{53}, env{42});
        static_assert(std::same_as<decltype(alloc), allocator>);
        ASSERT(alloc == allocator{53});
        ASSERT(test_std::get_allocator(ev) == allocator{53});
    }
    {
        test_std::inplace_stop_source source;
        auto [alloc, ev] = test_detail::spawn_future_get_allocator(
            alloc_sender{53}, test_std::prop(test_std::get_stop_token, source.get_token()));
        static_assert(std::same_as<decltype(alloc), allocator>);
        ASSERT(alloc == allocator{53});
        ASSERT(test_std::get_allocator(ev) == allocator{53});
        ASSERT(test_std::get_stop_token(ev) == source.get_token());
    }
    {
        test_std::inplace_stop_source source;
        auto [alloc, ev] = test_detail::spawn_future_get_allocator(
            alloc_sender{53},
            test_detail::join_env(test_std::prop(test_std::get_allocator, allocator(101)),
                                  test_std::prop(test_std::get_stop_token, source.get_token())));
        static_assert(std::same_as<decltype(alloc), allocator>);
        ASSERT(alloc == allocator{101});
        ASSERT(test_std::get_allocator(ev) == allocator{101});
        ASSERT(test_std::get_stop_token(ev) == source.get_token());
    }
    {
        auto [alloc, ev] = test_detail::spawn_future_get_allocator(sender{}, env{42});
        static_assert(std::same_as<decltype(alloc), std::allocator<void>>);
        static_assert(std::same_as<decltype(ev), env>);
        ASSERT(ev == env{42});
    }
}

} // namespace

TEST(exec_spawn_future) {
    static_assert(std::same_as<const test_std::spawn_future_t, decltype(test_std::spawn_future)>);
    test_spawn_future_interface<true>(sender{}, token<true>{});
    test_spawn_future_interface<false>(non_sender{}, token<true>{});
    test_spawn_future_interface<false>(sender{}, token<false>{});

    test_spawn_future_interface<false, non_env>(sender{}, token<true>{});

    test_state_base();
    test_receiver();

    test_get_allocator();

    test_spawn_future(sender{}, token<true>{}, env{});
}
