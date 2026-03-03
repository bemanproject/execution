// src/beman/execution/tests/exec-stopped-as-error.test.cpp             -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <concepts>
#include <optional>
#include <system_error>
#include <test/execution.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
import beman.execution.detail;
#else
#include <beman/test_std/detail/just.hpp>
#include <beman/test_std/detail/receiver.hpp>
#include <beman/test_std/detail/sender.hpp>
#include <beman/test_std/detail/stopped_as_optional.hpp>
#include <beman/test_std/detail/sync_wait.hpp>
#endif

// ----------------------------------------------------------------------------
namespace {
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

auto test_stopped_as_optional() -> void {
    test_std::sender auto sndr1 = test_std::just(42) | test_std::stopped_as_optional();
    using sigs_of_sndr1         = test_std::completion_signatures_of_t<decltype(sndr1), test_detail::sync_wait_env>;
    static_assert(
        std::same_as<sigs_of_sndr1, test_std::completion_signatures<test_std::set_value_t(std::optional<int>)>>);
    auto [i] = test_std::sync_wait(std::move(sndr1)).value();
    ASSERT(i == 42);

    test_std::sender auto sndr2 = test_std::stopped_as_optional(optional_sender<int>{});
    using sigs_of_sndr2 = test_std::completion_signatures_of_t<decltype(sndr2), test_std::detail::sync_wait_env>;
    static_assert(
        std::same_as<sigs_of_sndr2, test_std::completion_signatures<test_std::set_value_t(std::optional<int>)>>);
    auto [opt] = test_std::sync_wait(std::move(sndr2)).value();
    ASSERT(!opt.has_value());
}

} // namespace
// ----------------------------------------------------------------------------

TEST(exec_stopped_as_optional) { test_stopped_as_optional(); }
