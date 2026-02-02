// tests/beman/execution/exec-scope-simple-counting.test.cpp          -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <test/execution.hpp>
#include <concepts>
#include <tuple>
#include <type_traits>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution/detail/simple_counting_scope.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/just.hpp>
#include <beman/execution/detail/sync_wait.hpp>
#endif
#include <test/inline_scheduler.hpp>

// ----------------------------------------------------------------------------

namespace {

auto general() -> void {
    using scope = test_std::simple_counting_scope;
    using token = scope::token;

    // static_assert(requires(token const& tok){ { tok.wrap(test_std::just(10)) } noexcept; });
    static_assert(requires(const token& tok) {
        { tok.try_associate() } noexcept -> std::same_as<bool>;
    });
    static_assert(requires(const token& tok) { tok.try_associate(); });
    static_assert(requires(const token& tok) {
        { tok.disassociate() } noexcept;
    });

    static_assert(noexcept(scope{}));
    static_assert(!std::is_move_constructible_v<scope>);
    static_assert(!std::is_copy_constructible_v<scope>);
    static_assert(requires(scope sc) {
        { sc.get_token() } noexcept -> std::same_as<token>;
    });
    static_assert(requires(scope sc) {
        { sc.close() } noexcept -> std::same_as<void>;
    });
    static_assert(requires(scope sc) {
        { sc.join() } noexcept;
    });
}

struct join_receiver {
    using receiver_concept = test_std::receiver_t;

    struct env {
        auto query(const test_std::get_scheduler_t&) const noexcept -> test::inline_scheduler { return {}; }
    };

    bool& called;
    auto  set_value() && noexcept { this->called = true; }
    auto  get_env() const noexcept -> env { return {}; }
};

auto ctor() -> void {
    {
        test_std::simple_counting_scope scope;
    }
    test::death([] {
        test_std::simple_counting_scope scope;
        scope.get_token().try_associate();
    });
    test::death([] {
        test_std::simple_counting_scope scope;
        scope.get_token().try_associate();
        bool called{false};
        auto state(test_std::connect(scope.join(), join_receiver{called}));
        test_std::start(state);
    });
    test::death([] {
        test_std::simple_counting_scope scope;
        scope.get_token().try_associate();
        scope.close();
    });
    test::death([] {
        test_std::simple_counting_scope scope;
        scope.get_token().try_associate();
        bool called{false};
        auto state(test_std::connect(scope.join(), join_receiver{called}));
        test_std::start(state);
        scope.close();
    });
    {
        test_std::simple_counting_scope scope;
        scope.close();
    }
    {
        test_std::simple_counting_scope scope;
        scope.join();
    }
}

auto mem() -> void {
    {

        test_std::simple_counting_scope        scope;
        test_std::simple_counting_scope::token token{scope.get_token()};

        ASSERT(true == token.try_associate());
        token.disassociate();
        scope.close();
        ASSERT(false == token.try_associate());

        test_std::sync_wait(scope.join());
    }
    {
        test_std::simple_counting_scope scope;
        ASSERT(true == scope.get_token().try_associate());
        bool called{false};
        ASSERT(called == false);
        auto state(test_std::connect(scope.join(), join_receiver{called}));
        ASSERT(called == false);
        test_std::start(state);
        ASSERT(called == false);

        scope.get_token().disassociate();
        ASSERT(called == true);
    }
}
auto token() -> void {
    test_std::simple_counting_scope scope;
    const auto                      tok{scope.get_token()};
    auto                            sndr{tok.wrap(test_std::just(10))};
    static_assert(std::same_as<decltype(sndr), decltype(test_std::just(10))>);

    ASSERT(true == tok.try_associate());
    bool called{false};
    auto state(test_std::connect(scope.join(), join_receiver{called}));
    test_std::start(state);
    ASSERT(false == called);
    scope.close();
    ASSERT(false == called);
    ASSERT(false == tok.try_associate());
    ASSERT(false == called);
    tok.disassociate();
    ASSERT(true == called);
}

} // namespace

TEST(exec_scope_simple_counting) {
    general();
    ctor();
    mem();
    token();
}
