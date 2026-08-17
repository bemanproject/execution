// src/beman/execution/tests/exec-scope-snd-concepts.test.cpp                     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <test/execution.hpp>
#include <test/completion_test.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution/detail/enter_scope_sender.hpp>
#include <beman/execution/detail/enter_scope_sender_in.hpp>
#include <beman/execution/detail/exit_scope_sender_in.hpp>
#include <beman/execution/detail/exit_scope_sender.hpp>
#include <beman/execution/detail/just.hpp>
#endif

// ----------------------------------------------------------------------------

namespace {
template <typename... Completions>
struct sender {
    using sender_concept        = test_std::sender_tag;
    using completion_signatures = test_std::completion_signatures<Completions...>;
    template <typename, typename...>
    static consteval auto get_completion_signatures() -> completion_signatures {
        return {};
    }
};
} // namespace

TEST(exec_scope_snd_concepts) {
    // any sender models `exit_scope_sender`.
    static_assert(test_std::exit_scope_sender<decltype(test_std::just())>);
    static_assert(test_std::exit_scope_sender<decltype(test_std::just(1))>);
    static_assert(test_std::exit_scope_sender<decltype(test_std::just(1, 2))>);
    // no-value sender
    static_assert(test_std::exit_scope_sender_in<decltype(test_std::just()), test_std::env<>>);
    // sender with value completion doesn't model `exit_scope_sender_in`
    static_assert(!test_std::exit_scope_sender_in<decltype(test_std::just(1)), test_std::env<>>);

    // any sender models `enter_scope_sender`.
    static_assert(test_std::enter_scope_sender<decltype(test_std::just())>);
    static_assert(test_std::enter_scope_sender<decltype(test_std::just(1))>);
    static_assert(test_std::enter_scope_sender<decltype(test_std::just(1, 2))>);

    // simple senders don't model `enter_scope_sender_in`
    static_assert(!test_std::enter_scope_sender_in<decltype(test_std::just())>);
    static_assert(!test_std::enter_scope_sender_in<decltype(test_std::just(1))>);
    static_assert(!test_std::enter_scope_sender_in<decltype(test_std::just(1, 2))>);

    // senders producing senders
    static_assert(test_std::enter_scope_sender_in<decltype(test_std::just(test_std::just())), test_std::env<>>);
    static_assert(!test_std::enter_scope_sender_in<decltype(test_std::just(test_std::just(1))), test_std::env<>>);
    static_assert(
        !test_std::enter_scope_sender_in<decltype(test_std::just(test_std::just_error(1))), test_std::env<>>);
    static_assert(
        !test_std::enter_scope_sender_in<decltype(test_std::just(test_std::just_stopped())), test_std::env<>>);

    // extracting the exit sender
    static_assert(
        std::is_same_v<decltype(test_std::just()),
                       test_std::exit_scope_sender_of_t<decltype(test_std::just(test_std::just())), test_std::env<>>>);

    // different completion signatures for enter senders
    using just_sender_t = decltype(test_std::just());
    using t1            = sender<>;
    static_assert(!test_std::enter_scope_sender_in<t1, test_std::env<>>);

    using t2 = sender<test_std::set_value_t(just_sender_t)>;
    static_assert(test_std::enter_scope_sender_in<t2, test_std::env<>>);
    static_assert(std::is_same_v<test_std::exit_scope_sender_of_t<t2, test_std::env<>>, just_sender_t>);

    using t3 = sender<test_std::set_value_t(just_sender_t), test_std::set_error_t(int), test_std::set_stopped_t()>;
    static_assert(test_std::enter_scope_sender_in<t3, test_std::env<>>);
    static_assert(std::is_same_v<test_std::exit_scope_sender_of_t<t3, test_std::env<>>, just_sender_t>);

    using t4 = sender<test_std::set_value_t(just_sender_t),
                      test_std::set_value_t(int),
                      test_std::set_error_t(int),
                      test_std::set_stopped_t()>;
    static_assert(!test_std::enter_scope_sender_in<t4, test_std::env<>>);

    using t5 = sender<test_std::set_value_t(int), test_std::set_error_t(int), test_std::set_stopped_t()>;
    static_assert(!test_std::enter_scope_sender_in<t5, test_std::env<>>);
}
