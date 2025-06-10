// tests/beman/execution/exec-spawn.test.cpp                          -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/detail/spawn.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/receiver.hpp>
#include <beman/execution/detail/scope_token.hpp>
#include <beman/execution/detail/set_value.hpp>
#include <beman/execution/detail/set_stopped.hpp>
#include <test/execution.hpp>
#include <concepts>
#include <type_traits>
#include <utility>
#include <memory>

// ----------------------------------------------------------------------------

namespace {
struct env {};

template <bool B>
struct sender {
    using sender_concept        = std::conditional_t<B, test_std::sender_t, void>;
    using completion_signatures = test_std::completion_signatures<test_std::set_value_t()>;

    struct base {
        virtual ~base()                 = default;
        virtual auto complete() -> void = 0;
    };

    template <test_std::receiver Rcvr>
    struct state : base {
        std::remove_cvref_t<Rcvr> rcvr;
        base*&                    handle;

        using operation_state_concept = test_std::operation_state_t;
        state(auto&& r, base*& h) : rcvr(std::forward<decltype(r)>(r)), handle(h) {}
        auto start() & noexcept { this->handle = this; }
        auto complete() -> void override { test_std::set_value(std::move(this->rcvr)); }
    };

    base** handle{};
    template <test_std::receiver Rcvr>
    auto connect(Rcvr&& rcvr) -> state<Rcvr> {
        return state<Rcvr>{std::forward<Rcvr>(rcvr), *this->handle};
    }
};
static_assert(test_std::sender<sender<true>>);
static_assert(!test_std::sender<sender<false>>);

template <bool B>
struct token {
    bool* associated{};
    bool* disassociated{};
    bool  can_associate{true};
    auto  try_associate() noexcept(B) -> bool { return this->can_associate && (*this->associated = true); }
    auto  disassociate() noexcept(B) { *this->disassociated = true; }
    template <test_std::sender Sndr>
    auto wrap(Sndr&& sndr) {
        return std::forward<Sndr>(sndr);
    }
};
static_assert(test_std::scope_token<token<true>>);
static_assert(!test_std::scope_token<token<false>>);

template <bool Expect, typename Sndr, typename Tok, typename Ev>
auto test_overload(Sndr&& sndr, Tok&& tok, Ev ev) -> void {
    static_assert(Expect == requires { test_std::spawn(::std::forward<Sndr>(sndr), ::std::forward<Tok>(tok)); });
    static_assert(Expect == requires {
        test_std::spawn(::std::forward<Sndr>(sndr), ::std::forward<Tok>(tok), ::std::forward<Ev>(ev));
    });
}

struct test_state : test_detail::spawn_t::state_base {
    bool& value;
    test_state(bool& v) : value(v) {}
    auto complete() noexcept -> void override { value = true; }
};

auto test_spawn_state_base() -> void {
    bool       called{false};
    test_state state(called);
    ASSERT(called == false);
    test_detail::spawn_t::state_base& base{state};
    ASSERT(called == false);
    base.complete();
    ASSERT(called == true);
}

template <bool Expect, typename Rcvr>
auto test_spawn_receiver() -> void {
    static_assert(test_std::receiver<Rcvr>);
    static_assert(Expect == requires(Rcvr rcvr) { test_std::set_value(std::move(rcvr)); });
    static_assert(Expect != requires(Rcvr rcvr) { test_std::set_value(rcvr); });
    static_assert(Expect == requires(Rcvr rcvr) { test_std::set_stopped(std::move(rcvr)); });
    static_assert(Expect != requires(Rcvr rcvr) { test_std::set_stopped(rcvr); });

    {
        bool       called{false};
        test_state state(called);
        Rcvr       rcvr{&state};
        ASSERT(called == false);
        test_std::set_value(std::move(rcvr));
        ASSERT(called == true);
    }
    {
        bool       called{false};
        test_state state(called);
        Rcvr       rcvr{&state};
        ASSERT(called == false);
        test_std::set_stopped(std::move(rcvr));
        ASSERT(called == true);
    }
}

template <typename Alloc, test_std::scope_token Tok, test_std::sender Sndr>
auto test_spawn_state(Alloc&& alloc, Tok&& tok, Sndr&& sndr) -> void {
    static_assert(requires(test_detail::spawn_t::state<Alloc, Tok, Sndr> state) {
        static_cast<test_detail::spawn_t::state_base&>(state);
        test_detail::spawn_t::state<Alloc, Tok, Sndr>(alloc, std::forward<Sndr>(sndr), std::forward<Tok>(tok));
    });
}

auto test_spawn() {
    {
        sender<true>::base* handle{nullptr};
        bool                associated(false);
        bool                disassociated(false);
        ASSERT(handle == nullptr);
        ASSERT(associated == false);
        ASSERT(disassociated == false);
        test_std::spawn(sender<true>{&handle}, token<true>{&associated, &disassociated});
        ASSERT(handle != nullptr);
        ASSERT(associated == true);
        ASSERT(disassociated == false);
        handle->complete();
        ASSERT(handle != nullptr);
        ASSERT(associated == true);
        ASSERT(disassociated == true);
    }
    {
        sender<true>::base* handle{nullptr};
        bool                associated(false);
        bool                disassociated(false);
        ASSERT(handle == nullptr);
        ASSERT(associated == false);
        ASSERT(disassociated == false);
        test_std::spawn(sender<true>{&handle}, token<true>{&associated, &disassociated, false});
        ASSERT(handle == nullptr);
        ASSERT(associated == false);
        ASSERT(disassociated == false);
    }
}

} // namespace

TEST(exec_spawn_future) {
    static_assert(std::same_as<decltype(test_std::spawn), const test_std::spawn_t>);
    test_overload<true>(sender<true>{}, token<true>{}, env{});
    test_overload<false>(sender<false>{}, token<true>{}, env{});
    test_overload<false>(sender<true>{}, token<false>{}, env{});

    test_spawn_state_base();
    test_spawn_receiver<true, test_std::spawn_t::receiver>();
    test_spawn_state(std::allocator<void>{}, token<true>{}, sender<true>{});

    test_spawn();
}
