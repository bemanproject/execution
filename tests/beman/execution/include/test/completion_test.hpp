// tests/beman/execution/include/test/completion_test.hpp             -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_TESTS_BEMAN_EXECUTION_INCLUDE_TEST_COMPLETION_TEST
#define INCLUDED_TESTS_BEMAN_EXECUTION_INCLUDE_TEST_COMPLETION_TEST

#include <beman/execution/detail/common.hpp>
#include <test/execution.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <algorithm>
#include <array>
#include <atomic>
#include <cassert>
#include <chrono>
#include <concepts>
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <mutex>
#include <ranges>
#include <thread>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail.completion_signatures;
import beman.execution.detail.connect;
import beman.execution.detail.env_of_t;
import beman.execution.detail.get_completion_signatures;
import beman.execution.detail.operation_state;
import beman.execution.detail.receiver;
import beman.execution.detail.sender;
import beman.execution.detail.set_error;
import beman.execution.detail.set_stopped;
import beman.execution.detail.set_value;
import beman.execution.detail.start;
#else
#include <beman/execution/detail/completion_signatures.hpp>
#include <beman/execution/detail/connect.hpp>
#include <beman/execution/detail/env_of_t.hpp>
#include <beman/execution/detail/get_completion_signatures.hpp>
#include <beman/execution/detail/operation_state.hpp>
#include <beman/execution/detail/receiver.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/set_error.hpp>
#include <beman/execution/detail/set_stopped.hpp>
#include <beman/execution/detail/set_value.hpp>
#include <beman/execution/detail/start.hpp>
#endif

// ----------------------------------------------------------------------------

namespace test {
struct completion_test_base {
    virtual auto complete() const noexcept -> void = 0;
};
template <typename>
struct completion_test_function;

template <typename... T>
struct completion_test_function<::beman::execution::set_value_t(T...)> : completion_test_base {
    auto set_value(T...) const noexcept -> void { this->complete(); }
};
template <typename E>
struct completion_test_function<::beman::execution::set_error_t(E)> : completion_test_base {
    auto set_error(E) const noexcept -> void { this->complete(); }
};
template <>
struct completion_test_function<::beman::execution::set_stopped_t()> : completion_test_base {
    auto set_stopped() const noexcept -> void { this->complete(); }
};

template <typename>
struct completion_test_receiver;
template <typename... Sigs>
struct completion_test_receiver<::beman::execution::completion_signatures<Sigs...>>
    : completion_test_function<Sigs>... {};

template <::beman::execution::sender Sender>
struct completion_test {
    using sender_concept = ::beman::execution::sender_t;
    template <typename...>
    static consteval auto get_completion_signatures() noexcept {
        return ::beman::execution::completion_signatures<::beman::execution::set_value_t()>();
    }

    template <::beman::execution::receiver Receiver>
    struct state {
        using operation_state_concept = ::beman::execution::operation_state_t;
        struct inner_receiver
            : test::completion_test_receiver<
                  decltype(::beman::execution::get_completion_signatures<Sender,
                                                                         ::beman::execution::env_of_t<Receiver>>())> {
            using receiver_concept = ::beman::execution::receiver_t;
            state* st;
            auto   complete() const noexcept -> void override {
                ::beman::execution::set_value(std::move(this->st->receiver));
            }
            inner_receiver(state* s) : st(s) {}
        };
        using inner_state_t =
            decltype(::beman::execution::connect(std::declval<Sender>(), std::declval<inner_receiver>()));

        std::remove_cvref_t<Receiver> receiver;
        inner_state_t                 inner_state;

        template <::beman::execution::receiver R, ::beman::execution::sender S>
        state(R&& r, S&& s)
            : receiver(std::forward<R>(r)),
              inner_state(::beman::execution::connect(std::forward<S>(s), inner_receiver(this))) {}
        auto start() noexcept { ::beman::execution::start(this->inner_state); }
    };

    Sender sender;

    template <::beman::execution::receiver Receiver>
    auto connect(Receiver&& r) && noexcept {
        return state<Receiver>{std::forward<Receiver>(r), std::move(this->sender)};
    }
};

template <::beman::execution::sender Sender>
completion_test(Sender&& sender) -> completion_test<std::remove_cvref_t<Sender>>;
} // namespace test

// ----------------------------------------------------------------------------

#endif
