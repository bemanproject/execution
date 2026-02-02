// src/beman/execution/tests/exec-recv.test.cpp                     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/execution.hpp>
#include <test/execution.hpp>

// ----------------------------------------------------------------------------

namespace {
struct base {};
struct derived : test_std::receiver_t {};

struct no_receiver_concept {};
struct receiver_concept_not_deriving_from_receiver_t {
    using receiver_concept = base;
};

struct no_get_env;
struct non_env {
    friend struct no_get_env;

    non_env()                                  = default;
    non_env(non_env&&)                         = default;
    non_env(const non_env&)                    = default;
    auto operator=(non_env&&) -> non_env&      = default;
    auto operator=(const non_env&) -> non_env& = default;

  private:
    ~non_env() {}
};
struct no_get_env {
    using receiver_concept = test_std::receiver_t;
    auto get_env() const noexcept -> const non_env& {
        static non_env rc;
        return rc;
    }
};

struct not_move_constructible {
    using receiver_concept                                                   = test_std::receiver_t;
    not_move_constructible()                                                 = default;
    not_move_constructible(const not_move_constructible&)                    = default;
    not_move_constructible(not_move_constructible&&)                         = delete;
    ~not_move_constructible()                                                = default;
    auto operator=(const not_move_constructible&) -> not_move_constructible& = default;
    auto operator=(not_move_constructible&&) -> not_move_constructible&      = delete;
};
struct not_copy_constructible {
    using receiver_concept                                                   = test_std::receiver_t;
    not_copy_constructible()                                                 = default;
    not_copy_constructible(const not_copy_constructible&)                    = delete;
    not_copy_constructible(not_copy_constructible&&)                         = default;
    ~not_copy_constructible()                                                = default;
    auto operator=(const not_copy_constructible&) -> not_copy_constructible& = delete;
    auto operator=(not_copy_constructible&&) -> not_copy_constructible&      = default;
};

struct receiver_final final {
    using receiver_concept = test_std::receiver_t;
};
struct receiver_base {
    using receiver_concept = test_std::receiver_t;
};
struct receiver_derived {
    using receiver_concept = derived;
};

auto test_receiver_concept() -> void {
    static_assert(not test_std::receiver<no_receiver_concept>);
    static_assert(not test_std::receiver<receiver_concept_not_deriving_from_receiver_t>);
    static_assert(not test_std::receiver<no_get_env>);
    static_assert(not test_std::receiver<not_move_constructible>);
    static_assert(not test_std::receiver<const not_copy_constructible&>);
    static_assert(not test_std::receiver<not_copy_constructible&>);
    static_assert(not test_std::receiver<receiver_final>);

    static_assert(test_std::receiver<receiver_base>);
    static_assert(test_std::receiver<receiver_derived>);
}

template <int>
struct arg_t {};

struct receiver1 {
    auto set_value(arg_t<0>) noexcept -> void {}
    auto set_value(arg_t<2>) -> void {}
    auto set_error(arg_t<1>) noexcept -> void {}
    auto set_error(int) -> void {}
    auto set_stopped() noexcept -> void {}
};

auto test_valid_completions_for_concept() -> void {
    test_std::set_value(receiver1{}, arg_t<0>());
    static_assert(test_detail::callable<test_std::set_value_t, receiver1, arg_t<0>>);
    static_assert(not test_detail::callable<test_std::set_value_t, receiver1, int>);
    static_assert(not test_detail::callable<test_std::set_value_t, receiver1, arg_t<1>>);
    static_assert(test_detail::valid_completion_for<auto(arg_t<0>)->test_std::set_value_t, receiver1>);
    static_assert(not test_detail::valid_completion_for<auto(arg_t<1>)->test_std::set_value_t, receiver1>);
    static_assert(not test_detail::valid_completion_for<auto(arg_t<2>)->test_std::set_value_t, receiver1>);

    static_assert(test_detail::valid_completion_for<auto(arg_t<1>)->test_std::set_error_t, receiver1>);
    static_assert(not test_detail::valid_completion_for<auto(int)->test_std::set_error_t, receiver1>);

    static_assert(test_detail::valid_completion_for<auto()->test_std::set_stopped_t, receiver1>);
}
} // namespace

TEST(exec_recv) {
    test_receiver_concept();
    test_valid_completions_for_concept();
}
