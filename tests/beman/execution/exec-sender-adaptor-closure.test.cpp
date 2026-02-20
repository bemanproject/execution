// tests/beman/execution/exec-sender-adaptor-closure.test.cpp      -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <utility>

#include <test/execution.hpp>

#ifdef BEMAN_HAS_MODULES
import beman.execution;
import beman.execution.detail;
#else
#include <beman/execution/execution.hpp>
#endif

namespace {

// test helpers
template <class Sender>
struct wrapping_sender {
    using sender_concept = test_std::sender_t;
    Sender inner;

    template <class Self, class Rcvr>
    static auto connect(Self&& self, Rcvr&& rcvr) {
        return test_std::connect(std::forward<Self>(self).inner, std::forward<Rcvr>(rcvr));
    }

    template <class Env = test_std::env<>>
    using completion_signatures = test_std::completion_signatures_of_t<Sender, Env>;
};
struct wrapping_closure : test_std::sender_adaptor_closure<wrapping_closure> {
    template <test_std::sender Sender>
    auto operator()(Sender&& sndr) const {
        return wrapping_sender<std::decay_t<Sender>>{std::forward<Sender>(sndr)};
    }
};
struct identity_closure : test_std::sender_adaptor_closure<identity_closure> {
    template <test_std::sender Sender>
    auto operator()(Sender&& sndr) const {
        return std::forward<Sender>(sndr);
    }
};

struct adaptor_cpo : test_std::sender_adaptor_closure<adaptor_cpo> {
    auto operator()(auto&&... vals) const { return test_detail::make_sender_adaptor(*this, vals...); }

    template <test_std::sender Sender>
    auto operator()(Sender&& sndr, auto&&... vals) const {
        return test_detail::make_sender(
            *this, test_detail::product_type{::std::forward<decltype(vals)>(vals)...}, ::std::forward<Sender>(sndr));
    }
};

struct wrong_crtp_closure : test_std::sender_adaptor_closure<identity_closure> {};
struct double_inheritance_closure : identity_closure, test_std::sender_adaptor_closure<double_inheritance_closure> {};
struct extended_closure : identity_closure {};
struct both_sender_and_closure : test_std::sender_adaptor_closure<both_sender_and_closure> {
    using sender_concept = test_std::sender_t;
};

struct incomplete;
using incomplete_base = test_std::sender_adaptor_closure<incomplete>;
struct incomplete : incomplete_base {};
struct nothrow_closure : test_std::sender_adaptor_closure<nothrow_closure> {
    template <test_std::sender Sender>
    auto operator()(Sender&& sndr) const noexcept {
        return std::forward<Sender>(sndr);
    }
};

struct move_only_closure : test_std::sender_adaptor_closure<move_only_closure> {
    move_only_closure()                                    = default;
    move_only_closure(move_only_closure&&)                 = default;
    move_only_closure(const move_only_closure&)            = delete;
    move_only_closure& operator=(move_only_closure&&)      = default;
    move_only_closure& operator=(const move_only_closure&) = delete;

    template <test_std::sender Sender>
    auto operator()(Sender&& sndr) const {
        return std::forward<Sender>(sndr);
    }
};

struct lvalue_rvalue_closure : test_std::sender_adaptor_closure<lvalue_rvalue_closure> {
    template <test_std::sender Sender>
    auto operator()(Sender&& sndr) & {
        (void)sndr;
        return test_std::just(1); // lvalue result
    }

    template <test_std::sender Sender>
    auto operator()(Sender&& sndr) && {
        (void)sndr;
        return test_std::just(2.0); // rvalue result
    }
};

struct minimal_sender : decltype(test_std::just()){};

struct constrained_closure : test_std::sender_adaptor_closure<constrained_closure> {
    template <class Sender>
        requires std::same_as<std::decay_t<Sender>, minimal_sender>
    auto operator()(Sender&& sndr) const {
        return std::forward<Sender>(sndr);
    }
};

struct non_composable_closure : test_std::sender_adaptor_closure<non_composable_closure> {
    non_composable_closure()                                         = default;
    non_composable_closure(const non_composable_closure&)            = delete;
    non_composable_closure(non_composable_closure&&)                 = delete;
    non_composable_closure& operator=(const non_composable_closure&) = delete;
    non_composable_closure& operator=(non_composable_closure&&)      = delete;
};

// helper concepts
template <class Closure1, class Closure2>
concept can_compose_closures = requires(Closure1 c1, Closure2 c2) { c1 | c2; };

template <class Tag, class... Args>
concept can_make_adaptor =
    requires(Tag tag, Args... args) { test_detail::make_sender_adaptor(std::move(tag), std::move(args)...); };

struct non_movable {
    non_movable()                              = default;
    non_movable(const non_movable&)            = delete;
    non_movable(non_movable&&)                 = delete;
    non_movable& operator=(const non_movable&) = delete;
    non_movable& operator=(non_movable&&)      = delete;
};

template <class Sender, class Closure>
concept can_pipe = requires(Sender sndr, Closure closure) { sndr | closure; };

// test functions
auto test_basic_closure_validity() -> void {
    static_assert(test_detail::is_sender_adaptor_closure<identity_closure>, "identity_closure should pass");
    static_assert(not test_detail::is_sender_adaptor_closure<wrong_crtp_closure>, "wrong CRTP should fail");
    static_assert(not test_detail::is_sender_adaptor_closure<double_inheritance_closure>,
                  "non-unique base should fail");
    static_assert(not test_detail::is_sender_adaptor_closure<extended_closure>, "no direct CRTP should fail");
    static_assert(not test_detail::is_sender_adaptor_closure<both_sender_and_closure>,
                  "closure cannot also model sender");
    static_assert(test_detail::is_sender_adaptor_closure<incomplete>,
                  "incomplete CRTP base should work once completed");
}

auto test_basic_pipe_syntax() -> void {
    auto sndr    = test_std::just(1);
    auto adapted = sndr | wrapping_closure{};
    static_assert(std::same_as<decltype(adapted), wrapping_sender<decltype(sndr)>>);
}

auto test_composition_syntax() -> void {
    static_assert(can_compose_closures<wrapping_closure, wrapping_closure>);
    auto clos1    = wrapping_closure{};
    auto clos2    = wrapping_closure{};
    auto composed = clos1 | clos2;

    static_assert(test_detail::is_sender_adaptor_closure<decltype(composed)>);

    // snd | composed
    auto sndr    = test_std::just(1);
    auto adapted = sndr | composed;

    // should be wrapped twice: wrapper<wrapper<just>>
    using expected_t = wrapping_sender<wrapping_sender<decltype(sndr)>>;
    static_assert(std::same_as<decltype(adapted), expected_t>);
}

auto test_associativity() -> void {
    // (snd | c1) | c2 == snd | (c1 | c2)
    auto sndr  = test_std::just(1);
    auto clos1 = wrapping_closure{};
    auto clos2 = wrapping_closure{};

    auto res1 = (sndr | clos1) | clos2;
    auto res2 = sndr | (clos1 | clos2);

    static_assert(std::same_as<decltype(res1), decltype(res2)>);
}

auto test_pipe_equivalence() -> void {
    auto sndr    = test_std::just(1);
    using left_t = decltype(wrapping_closure{}(sndr));
    using pipe_t = decltype(sndr | wrapping_closure{});
    static_assert(std::same_as<left_t, pipe_t>);
}

auto test_partial_application() -> void {
    auto closure = adaptor_cpo{}(1995);
    static_assert(test_detail::is_sender_adaptor_closure<decltype(closure)>);

    auto sndr   = test_std::just(1);
    auto direct = closure(sndr);
    auto piped  = sndr | closure;
    static_assert(std::same_as<decltype(direct), decltype(piped)>);
    static_assert(test_std::sender<decltype(piped)>);
}

auto test_noexcept_propagation() -> void {
    auto sndr = test_std::just(1);

    // nothrow closure => pipe is noexcept
    static_assert(noexcept(sndr | nothrow_closure{}));
    static_assert(noexcept(nothrow_closure{}(sndr)));

    // non-noexcept closure => pipe is not noexcept
    static_assert(not noexcept(sndr | identity_closure{} | nothrow_closure{}));
    static_assert(not noexcept(identity_closure{}(sndr)));

    // composition
    static_assert(noexcept(nothrow_closure{} | nothrow_closure{}));
}

auto test_composed_call_pattern() -> void {
    auto sndr = test_std::just(1);

    // composed(sndr) should wrap twice: wrapping_sender<wrapping_sender<sndr>>
    auto composed = wrapping_closure{} | wrapping_closure{};
    auto result   = composed(sndr);

    using inner_t    = wrapping_sender<std::decay_t<decltype(sndr)>>;
    using expected_t = wrapping_sender<inner_t>;
    static_assert(std::same_as<decltype(result), expected_t>);

    // multi-level: c(b(a(arg)))
    auto abc             = wrapping_closure{} | wrapping_closure{} | wrapping_closure{};
    auto result_abc      = sndr | abc;
    using middle_t       = wrapping_sender<inner_t>;
    using expected_abc_t = wrapping_sender<middle_t>;
    static_assert(std::same_as<decltype(result_abc), expected_abc_t>);
}

auto test_partial_application_well_formedness() -> void {
    // make_adaptor with a non-movable arg should be ill-formed (SFINAE)
    static_assert(not can_make_adaptor<adaptor_cpo, non_movable>);

    // make_adaptor with a movable arg should be well-formed
    static_assert(requires(adaptor_cpo cpo) { test_detail::make_sender_adaptor(cpo, 999); });
}

auto test_move_only_closure() -> void {
    auto sndr = test_std::just(1);

    // direct
    auto c1 = move_only_closure{};
    (void)std::move(c1)(sndr);

    // pipe
    auto c2 = move_only_closure{};
    (void)(sndr | std::move(c2));

    // composition
    auto composed = move_only_closure{} | move_only_closure{};
    (void)(sndr | std::move(composed));
}

auto test_ref_qualification_propagation() -> void {
    auto sndr = test_std::just(1);

    // direct application
    auto c1 = lvalue_rvalue_closure{};
    static_assert(std::same_as<decltype(c1(sndr)), decltype(test_std::just(1))>);
    static_assert(std::same_as<decltype(std::move(c1)(sndr)), decltype(test_std::just(2.0))>);

    // pipe application
    auto c2 = lvalue_rvalue_closure{};
    static_assert(std::same_as<decltype(sndr | c2), decltype(test_std::just(1))>);
    static_assert(std::same_as<decltype(sndr | std::move(c2)), decltype(test_std::just(2.0))>);

    // composition
    auto composed_l = lvalue_rvalue_closure{} | identity_closure{};
    static_assert(std::same_as<decltype(composed_l(sndr)), decltype(test_std::just(1))>);

    auto composed_r = lvalue_rvalue_closure{} | identity_closure{};
    static_assert(std::same_as<decltype(std::move(composed_r)(sndr)), decltype(test_std::just(2.0))>);
}

auto test_multi_arg_adaptor() -> void {
    auto sndr    = test_std::just();
    auto closure = adaptor_cpo{}(42, 3.14);

    static_assert(test_detail::is_sender_adaptor_closure<decltype(closure)>);

    auto res = sndr | closure;
    (void)res;

    // Check movable arguments
    auto closure_moved = adaptor_cpo{}(42, 3.14);
    (void)(sndr | std::move(closure_moved));
}

auto test_sfinae_friendliness() -> void {
    // constrained_closure only accepts minimal_sender
    static_assert(can_pipe<minimal_sender, constrained_closure>);
    static_assert(not can_pipe<decltype(test_std::just()), constrained_closure>);

    // composition SFINAE: (constrained | identity)(just()) should fail
    static_assert(not can_pipe<decltype(test_std::just()), decltype(constrained_closure{} | identity_closure{})>);
}

auto test_composition_well_formedness() -> void {
    // non-copyable/non-movable closure cannot compose (spec §1: well-formed if initializations well-formed)
    static_assert(not can_compose_closures<non_composable_closure, non_composable_closure>);
    static_assert(not can_compose_closures<non_composable_closure, identity_closure>);
    static_assert(not can_compose_closures<identity_closure, non_composable_closure>);
}

} // namespace

TEST(exec_sender_adaptor_closure) {
    test_basic_closure_validity();
    test_basic_pipe_syntax();
    test_composition_syntax();
    test_associativity();
    test_pipe_equivalence();
    test_partial_application();
    test_noexcept_propagation();
    test_composed_call_pattern();
    test_partial_application_well_formedness();
    test_move_only_closure();
    test_ref_qualification_propagation();
    test_multi_arg_adaptor();
    test_sfinae_friendliness();
    test_composition_well_formedness();
}
