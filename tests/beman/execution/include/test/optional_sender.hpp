// tests/beman/execution/include/optional_sender.hpp                  -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_TESTS_BEMAN_EXECUTION_INCLUDE_OPTIONAL_SENDER
#define INCLUDED_TESTS_BEMAN_EXECUTION_INCLUDE_OPTIONAL_SENDER

#include <test/execution.hpp>
#include <optional>
#include <utility>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
import beman.execution.detail;
#else
#include <beman/execution/detail/just.hpp>
#include <beman/execution/detail/receiver.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/stopped_as_error.hpp>
#include <beman/execution/detail/sync_wait.hpp>
#endif

// ----------------------------------------------------------------------------

namespace test {
template <typename T>
struct optional_sender {
    using sender_concept        = test_std::sender_t;
    using completion_signatures = test_std::completion_signatures<test_std::set_value_t(T), test_std::set_stopped_t()>;

    optional_sender() = default;

    explicit optional_sender(T value) noexcept : opt(value) {}

    template <typename, typename...>
    static consteval auto get_completion_signatures() noexcept -> completion_signatures {
        return {};
    }

    template <test_std::receiver_of<completion_signatures> Rcvr>
    auto connect(Rcvr rcvr) && noexcept -> auto {
        struct state {
            using operation_state_concept = test_std::operation_state_t;
            auto start() & noexcept -> void {
                test::use_type<operation_state_concept>(); // make -Werror=unused-local-typedefs happy
                if (opt_) {
                    test_std::set_value(std::move(rcvr_), *opt_);
                } else {
                    test_std::set_stopped(std::move(rcvr_));
                }
            }

            Rcvr             rcvr_;
            std::optional<T> opt_;
        };
        return state{rcvr, std::move(opt)};
    }

    std::optional<T> opt;
};
} // namespace test

// ----------------------------------------------------------------------------

#endif
