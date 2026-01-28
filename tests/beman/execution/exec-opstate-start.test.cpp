// src/beman/execution/tests/exec-opstate-start.test.cpp            -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifdef BEMAN_USE_MODULE
import beman.execution;
#else
#include <beman/execution/execution.hpp>
#endif
#include <test/execution.hpp>

#include <utility>

// ----------------------------------------------------------------------------

namespace {
struct receiver {
    int& value;
    auto set_value(int val) && noexcept -> void { this->value = val; }
};

struct non_opstate {
    receiver rcvr;
};

template <bool Noexcept>
struct opstate {
    receiver rcvr;
    auto     start() const noexcept(Noexcept) -> void { test_std::set_value(receiver{this->rcvr.value}, 42); }
};

template <typename State>
auto test_start_argument_type() {
    int         value{};
    State       state{receiver{value}};
    int         cvalue{};
    const State cstate{receiver{cvalue}};

    static_assert(requires { test_std::start(state); });
    static_assert(requires { test_std::start(cstate); });

    static_assert(not requires { test_std::start(State(receiver{value})); });
    static_assert(not requires { test_std::start(std::move(state)); });
    static_assert(not requires { test_std::start(std::move(cstate)); });
}

template <typename State>
auto test_start_member() {
    int   value{};
    State state{receiver{value}};
    static_assert(not requires { test_std::start(state); });
    const State cstate{receiver{value}};
    static_assert(not requires { test_std::start(cstate); });
}

template <typename State>
auto test_start_noexcept() {
    int   value{};
    State state{receiver{value}};
    static_assert(noexcept(state));
    int         cvalue{};
    const State cstate{receiver{cvalue}};
    static_assert(noexcept(cstate));
}

template <typename State>
auto test_start_call() {
    int         value{};
    State       state{receiver{value}};
    int         cvalue{};
    const State cstate{receiver{cvalue}};

    ASSERT(value == 0);
    test_std::start(state);
    ASSERT(value == 42);

    ASSERT(cvalue == 0);
    test_std::start(cstate);
    ASSERT(cvalue == 42);
}
} // namespace

TEST(exec_opstate_start) {
    static_assert(std::same_as<const test_std::start_t, decltype(test_std::start)>);

    test_start_argument_type<opstate<true>>();
    test_start_member<non_opstate>();
    test_start_noexcept<opstate<false>>();
    test_start_call<opstate<true>>();
}
