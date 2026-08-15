// src/beman/execution/tests/exec-recv-concepts.test.cpp            -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <test/execution.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
import beman.execution.detail;
#else
#include <beman/execution/detail/has_completions.hpp>
#include <beman/execution/detail/inlinable_receiver.hpp>
#include <beman/execution/detail/receiver_of.hpp>
#include <beman/execution/detail/valid_completion_for.hpp>
#include <beman/execution.hpp>
#endif

// ----------------------------------------------------------------------------

namespace {
struct arg {};
struct error {};

struct not_a_receiver {
    auto get_env() const noexcept -> test_std::env<> { return {}; }
    auto set_value(auto&&...) && noexcept -> void {}
    auto set_error(auto&&) && noexcept -> void {}
    auto set_stopped() && noexcept -> void {}
};

struct final_receiver final {
    using receiver_concept = test_std::receiver_tag;
};

template <typename... T>
struct value_receiver {
    using receiver_concept = test_std::receiver_tag;
    auto set_value(T...) && noexcept -> void {}
};

template <typename T>
struct error_receiver {
    using receiver_concept = test_std::receiver_tag;
    auto set_error(T) && noexcept -> void {}
};

struct stopped_receiver {
    using receiver_concept = test_std::receiver_tag;
    auto set_stopped() && noexcept -> void {}
};

template <typename Concept = test_std::receiver_tag>
struct multi_receiver {
    using receiver_concept = Concept;

    auto set_value(int) && noexcept -> void {}
    auto set_value(int, arg) && noexcept -> void {}
    auto set_value(arg, arg) && noexcept -> void {}
    auto set_error(error) && noexcept -> void {}
    auto set_stopped() && noexcept -> void {}
};

auto test_valid_completion_for() -> void {
    static_assert(test_detail::valid_completion_for<test_std::set_value_t(), value_receiver<>>);
    static_assert(not test_detail::valid_completion_for<test_std::set_stopped_t(), value_receiver<>>);
    static_assert(not test_detail::valid_completion_for<test_std::set_value_t(arg), value_receiver<>>);
    static_assert(test_detail::valid_completion_for<test_std::set_value_t(arg), value_receiver<arg>>);
    static_assert(not test_detail::valid_completion_for<test_std::set_value_t(int, arg), value_receiver<int>>);
    static_assert(test_detail::valid_completion_for<test_std::set_value_t(int, arg), value_receiver<int, arg>>);
    static_assert(not test_detail::valid_completion_for<test_std::set_error_t(error), value_receiver<error>>);

    static_assert(not test_detail::valid_completion_for<test_std::set_stopped_t(), error_receiver<error>>);
    static_assert(not test_detail::valid_completion_for<test_std::set_value_t(error), error_receiver<error>>);
    static_assert(test_detail::valid_completion_for<test_std::set_error_t(error), error_receiver<error>>);
    static_assert(not test_detail::valid_completion_for<test_std::set_error_t(error), error_receiver<int>>);
    static_assert(not test_detail::valid_completion_for<test_std::set_error_t(int), error_receiver<error>>);

    static_assert(test_detail::valid_completion_for<test_std::set_stopped_t(), stopped_receiver>);
    static_assert(not test_detail::valid_completion_for<test_std::set_value_t(), stopped_receiver>);
}

auto test_has_completions() -> void {
    static_assert(not test_std::receiver<not_a_receiver>);
    static_assert(not test_std::receiver<final_receiver>);
    static_assert(test_std::receiver<value_receiver<int>>);
    static_assert(test_detail::has_completions<value_receiver<int>, test_std::completion_signatures<>>);
    static_assert(test_detail::has_completions<value_receiver<int>,
                                               test_std::completion_signatures<test_std::set_value_t(int)>>);
    static_assert(not test_detail::has_completions<value_receiver<int>,
                                                   test_std::completion_signatures<test_std::set_value_t(int, int)>>);

    static_assert(test_std::receiver<error_receiver<int>>);
    static_assert(test_detail::has_completions<error_receiver<int>,
                                               test_std::completion_signatures<test_std::set_error_t(int)>>);
    static_assert(not test_detail::has_completions<error_receiver<int>,
                                                   test_std::completion_signatures<test_std::set_error_t(error)>>);
    static_assert(test_detail::has_completions<error_receiver<error>,
                                               test_std::completion_signatures<test_std::set_error_t(error)>>);

    static_assert(test_std::receiver<stopped_receiver>);
    static_assert(not test_detail::has_completions<error_receiver<error>,
                                                   test_std::completion_signatures<test_std::set_stopped_t()>>);
    static_assert(
        test_detail::has_completions<stopped_receiver, test_std::completion_signatures<test_std::set_stopped_t()>>);

    static_assert(test_std::receiver<stopped_receiver>);
    static_assert(test_detail::has_completions<multi_receiver<>,
                                               test_std::completion_signatures<test_std::set_value_t(int),
                                                                               test_std::set_value_t(int, arg),
                                                                               test_std::set_value_t(arg, arg),
                                                                               test_std::set_error_t(error),
                                                                               test_std::set_stopped_t()>>);
    static_assert(not test_detail::has_completions<multi_receiver<>,
                                                   test_std::completion_signatures<test_std::set_value_t(int),
                                                                                   test_std::set_value_t(arg, int),
                                                                                   test_std::set_value_t(arg, arg),
                                                                                   test_std::set_error_t(error),
                                                                                   test_std::set_stopped_t()>>);
    static_assert(not test_detail::has_completions<multi_receiver<>,
                                                   test_std::completion_signatures<test_std::set_value_t(int),
                                                                                   test_std::set_value_t(int, arg),
                                                                                   test_std::set_value_t(arg, arg),
                                                                                   test_std::set_error_t(int),
                                                                                   test_std::set_stopped_t()>>);
}

auto test_receiver_of() -> void {
    static_assert(test_std::receiver_of<value_receiver<int>, test_std::completion_signatures<>>);
    static_assert(
        test_std::receiver_of<value_receiver<int>, test_std::completion_signatures<test_std::set_value_t(int)>>);
    static_assert(not test_std::receiver_of<value_receiver<int>,
                                            test_std::completion_signatures<test_std::set_value_t(int, int)>>);

    static_assert(
        test_std::receiver_of<error_receiver<int>, test_std::completion_signatures<test_std::set_error_t(int)>>);
    static_assert(
        not test_std::receiver_of<error_receiver<int>, test_std::completion_signatures<test_std::set_error_t(error)>>);
    static_assert(
        test_std::receiver_of<error_receiver<error>, test_std::completion_signatures<test_std::set_error_t(error)>>);

    static_assert(
        not test_std::receiver_of<error_receiver<error>, test_std::completion_signatures<test_std::set_stopped_t()>>);
    static_assert(test_std::receiver_of<stopped_receiver, test_std::completion_signatures<test_std::set_stopped_t()>>);

    static_assert(test_std::receiver_of<multi_receiver<>,
                                        test_std::completion_signatures<test_std::set_value_t(int),
                                                                        test_std::set_value_t(int, arg),
                                                                        test_std::set_value_t(arg, arg),
                                                                        test_std::set_error_t(error),
                                                                        test_std::set_stopped_t()>>);
    static_assert(not test_std::receiver_of<multi_receiver<int>,
                                            test_std::completion_signatures<test_std::set_value_t(int),
                                                                            test_std::set_value_t(int, arg),
                                                                            test_std::set_value_t(arg, arg),
                                                                            test_std::set_error_t(error),
                                                                            test_std::set_stopped_t()>>);
    static_assert(not test_std::receiver_of<multi_receiver<>,
                                            test_std::completion_signatures<test_std::set_value_t(int),
                                                                            test_std::set_value_t(arg, int),
                                                                            test_std::set_value_t(arg, arg),
                                                                            test_std::set_error_t(error),
                                                                            test_std::set_stopped_t()>>);
    static_assert(not test_std::receiver_of<multi_receiver<>,
                                            test_std::completion_signatures<test_std::set_value_t(int),
                                                                            test_std::set_value_t(int, arg),
                                                                            test_std::set_value_t(arg, arg),
                                                                            test_std::set_error_t(int),
                                                                            test_std::set_stopped_t()>>);
}

struct inlinable_state {};
struct non_inlinable_state {};
struct receiver { using receiver_concept = test_std::receiver_tag; };

struct inlinable_receiver {
    using receiver_concept = test_std::receiver_tag;
    static auto make_receiver_for(inlinable_state& child) noexcept -> inlinable_receiver {
        return inlinable_receiver{};
    }
    static auto make_receiver_for(non_inlinable_state& child) noexcept -> receiver {
        return receiver{};
    }
};
static_assert(test_std::receiver<inlinable_receiver>);

auto test_inlinable_receiver() -> void {
    static_assert(not test_std::inlinable_receiver<inlinable_receiver, int>);
    static_assert(not test_std::inlinable_receiver<inlinable_receiver, non_inlinable_state>);
    static_assert(test_std::inlinable_receiver<inlinable_receiver, inlinable_state>);
}
} // namespace

TEST(exec_recv_concepts) {
    test_valid_completion_for();
    test_has_completions();
    test_receiver_of();
    test_inlinable_receiver();
}
