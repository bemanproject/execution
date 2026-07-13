// src/beman/execution/tests/exec-within.test.cpp                     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <stdexcept>
#include <string>
#include <tuple>
#include <test/execution.hpp>
#include <test/completion_test.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution/detail/within.hpp>
#include <beman/execution/detail/just.hpp>
#include <beman/execution/detail/then.hpp>
#include <beman/execution/detail/sync_wait.hpp>
#include <beman/execution/detail/enter_scope_sender_in.hpp>
#endif

// ----------------------------------------------------------------------------

/// Returns a scope that does nothing and completes immediately.
auto empty_scope() -> test_std::sender auto {
    auto r = test_std::just(test_std::just());
    static_assert(test_std::enter_scope_sender_in<decltype(r), test_std::env<>>);
    return r;
}

/// Returns a scope that sets `enter_executed` to true when entered and sets `exit_executed` to true when exited.
auto tracing_scope(bool& enter_executed, bool& exit_executed) -> test_std::sender auto {
    test_std::sender auto exit_sender =
        test_std::just() | test_std::then([&exit_executed]() noexcept { exit_executed = true; });
    test_std::sender auto r =
        test_std::just() | test_std::then([&enter_executed, exit_sender]() noexcept -> decltype(exit_sender) {
            enter_executed = true;
            return exit_sender;
        });
    static_assert(test_std::enter_scope_sender_in<decltype(r), test_std::env<>>);
    return r;
}

/// Checks that we can make a trivial invocation of `within` and that it behaves as expected.
auto test_trivial_invocation() -> void {
    // Arrange
    bool                  executed{false};
    test_std::sender auto scope = empty_scope();
    test_std::sender auto work  = test_std::just() | test_std::then([&executed]() noexcept { executed = true; });

    // Act
    test_std::sync_wait(test_std::within(std::move(scope), std::move(work)));

    // Assert
    ASSERT(executed);
}

/// Checks that scope action is properly executed when using `within`.
auto test_scope_use() -> void {

    // Arrange
    bool                  executed{false};
    bool                  enter_executed{false};
    bool                  exit_executed{false};
    test_std::sender auto scope = tracing_scope(enter_executed, exit_executed);
    test_std::sender auto work  = test_std::just() | test_std::then([&executed]() noexcept { executed = true; });

    // Act
    test_std::sync_wait(test_std::within(std::move(scope), std::move(work)));

    // Assert
    ASSERT(executed);
    ASSERT(enter_executed);
    ASSERT(exit_executed);
}

/// Checks that the completion values of work is properly propagated when using `within`.
auto test_work_completes_with_value() -> void {

    // Arrange
    bool                  enter_executed{false};
    bool                  exit_executed{false};
    test_std::sender auto scope = tracing_scope(enter_executed, exit_executed);
    test_std::sender auto work  = test_std::just(13, 17, 19);

    // Act
    auto r = test_std::sync_wait(test_std::within(std::move(scope), std::move(work)));

    // Assert
    ASSERT(enter_executed);
    ASSERT(exit_executed);
    ASSERT(r.has_value());
    ASSERT((*r == std::tuple{13, 17, 19}));
}

/// Sender with multiple value completion alternatives, but always completes with the int,int alternative.
struct sender_with_multiple_completions {
    using sender_concept        = test_std::sender_tag;
    using completion_signatures = test_std::completion_signatures<test_std::set_value_t(int, int),
                                                                  test_std::set_value_t(int, double),
                                                                  test_std::set_value_t(int, std::string),
                                                                  test_std::set_error_t(std::exception_ptr),
                                                                  test_std::set_stopped_t()>;
    template <typename, typename...>
    static consteval auto get_completion_signatures() -> completion_signatures {
        return {};
    }

    template <test_std::receiver Receiver>
    auto connect(Receiver&& rcvr) && noexcept {
        return test_std::connect(test_std::just(13, 17), std::forward<Receiver>(rcvr));
    }
};

/// Receiver that accepts all completions, but fails if the wrong completion is called.
struct int_int_receiver {
    using receiver_concept = test_std::receiver_tag;
    auto set_value(int x, int y) && noexcept -> void {
        ASSERT(x == 13);
        ASSERT(y == 17);
    }
    template <typename... Args>
    auto set_value(Args&&...) && noexcept -> void {
        ASSERT(!"generic set_value should not be called");
    }
    template <typename E>
    auto set_error(E&&) && noexcept -> void {
        ASSERT(!"set_error should not be called");
    }
    auto set_stopped() && noexcept -> void { ASSERT(!"set_stopped should not be called"); }
};

/// If the work has multiple value completion alternatives, check that the completion selection works appropriately.
auto test_work_with_multiple_values_alternatives() -> void {

    // Arrange
    bool                  enter_executed{false};
    bool                  exit_executed{false};
    test_std::sender auto scope = tracing_scope(enter_executed, exit_executed);
    test_std::sender auto work  = sender_with_multiple_completions{};

    // Act & assert
    test_std::sender auto s  = test_std::within(std::move(scope), std::move(work));
    auto                  op = test_std::connect(std::move(s), int_int_receiver{});
    test_std::start(op);

    // Assert
    ASSERT(enter_executed);
    ASSERT(exit_executed);
}

/// Checks that if the work completes with an error, the error is propagated and the exit scope is executed.
auto test_work_completes_with_error() -> void {

    // Arrange
    bool                  enter_executed{false};
    bool                  exit_executed{false};
    int                   result{0};
    test_std::sender auto scope = tracing_scope(enter_executed, exit_executed);
    test_std::sender auto work  = test_std::just_error(19);

    // Act
    test_std::sync_wait(test_std::within(std::move(scope), std::move(work)) |
                        test_std::upon_error([&result](int x) noexcept { result = x; }));

    // Assert
    ASSERT(enter_executed);
    ASSERT(exit_executed);
    ASSERT(result == 19);
}

/// Checks that if the work completes with stopped, the completion is propagated and the exit scope is executed.
auto test_work_completes_with_stopped() -> void {

    // Arrange
    bool                  enter_executed{false};
    bool                  exit_executed{false};
    bool                  work_stopped{false};
    test_std::sender auto scope = tracing_scope(enter_executed, exit_executed);
    test_std::sender auto work  = test_std::just_stopped();

    // Act
    test_std::sync_wait(test_std::within(std::move(scope), std::move(work)) |
                        test_std::upon_stopped([&work_stopped]() noexcept { work_stopped = true; }));

    // Assert
    ASSERT(enter_executed);
    ASSERT(exit_executed);
    ASSERT(work_stopped);
}

/// Checks that if the enter scope completes with error, the work isn't executed and the error is propagated.
auto test_scope_enter_completes_with_error() -> void {

    // Arrange
    bool                  enter_executed{false};
    bool                  exit_executed{false};
    bool                  work_executed{false};
    test_std::sender auto exit_sender =
        test_std::just() | test_std::then([&exit_executed]() noexcept { exit_executed = true; });
    test_std::sender auto scope =
        test_std::just() | test_std::then([&enter_executed, exit_sender]() -> decltype(exit_sender) {
            enter_executed = true;
            throw 13; // simulate error in enter scope
            return exit_sender;
        });
    test_std::sender auto work =
        test_std::just() | test_std::then([&work_executed]() noexcept { work_executed = true; });

    // Act & assert
    try {
        test_std::sync_wait(test_std::within(std::move(scope), std::move(work)));
        ASSERT(!"Expected exception to be thrown");
    } catch (int e) {
        ASSERT(e == 13);
    }

    // Assert
    ASSERT(enter_executed);
    ASSERT(!work_executed);
    ASSERT(!exit_executed);
}

/// Sender that can complete with value or stopped, but always completes with stopped.
struct sender_completing_with_stopped {
    using sender_concept        = test_std::sender_tag;
    using completion_signatures = test_std::completion_signatures<test_std::set_value_t(), test_std::set_stopped_t()>;
    template <typename, typename...>
    static consteval auto get_completion_signatures() -> completion_signatures {
        return {};
    }

    template <test_std::receiver Receiver>
    auto connect(Receiver&& rcvr) && noexcept {
        return test_std::connect(test_std::just_stopped(), std::forward<Receiver>(rcvr));
    }
};

/// Checks that if the enter scope completes with stopped, the work isn't executed and the stopped is propagated.
auto test_scope_enter_completes_with_stopped() -> void {

    // Arrange
    bool                  exit_executed{false};
    bool                  work_executed{false};
    bool                  work_stopped{false};
    test_std::sender auto exit_sender =
        test_std::just() | test_std::then([&exit_executed]() noexcept { exit_executed = true; });
    test_std::sender auto scope =
        sender_completing_with_stopped{} |
        test_std::then([exit_sender]() noexcept -> decltype(exit_sender) { return exit_sender; });
    test_std::sender auto work =
        test_std::just() | test_std::then([&work_executed]() noexcept { work_executed = true; });

    // Act
    test_std::sync_wait(test_std::within(std::move(scope), std::move(work)) |
                        test_std::upon_stopped([&work_stopped]() noexcept { work_stopped = true; }));

    // Assert
    ASSERT(!work_executed);
    ASSERT(!exit_executed);
    ASSERT(work_stopped);
}

TEST(exec_within) {
    static_assert(std::same_as<const test_std::within_t, decltype(test_std::within)>);

    test_trivial_invocation();
    test_scope_use();
    test_work_completes_with_value();
    test_work_with_multiple_values_alternatives();
    test_work_completes_with_error();
    test_work_completes_with_stopped();
    test_scope_enter_completes_with_error();
    test_scope_enter_completes_with_stopped();
}
