// src/beman/execution/tests/exec-stopped-as-error.test.cpp             -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <test/execution.hpp>
#include <test/sender_env.hpp>
#include <test/optional_sender.hpp>
#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <concepts>
#include <optional>
#include <utility>
#include <system_error>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution;
import beman.execution.detail;
#else
#include <beman/execution/detail/connect.hpp>
#include <beman/execution/detail/env.hpp>
#include <beman/execution/detail/get_completion_scheduler.hpp>
#include <beman/execution/detail/get_env.hpp>
#include <beman/execution/detail/get_stop_token.hpp>
#include <beman/execution/detail/inplace_stop_source.hpp>
#include <beman/execution/detail/just.hpp>
#include <beman/execution/detail/receiver.hpp>
#include <beman/execution/detail/run_loop.hpp>
#include <beman/execution/detail/schedule.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/start.hpp>
#include <beman/execution/detail/stopped_as_optional.hpp>
#include <beman/execution/detail/sync_wait.hpp>
#include <beman/execution/detail/then.hpp>
#endif

// ----------------------------------------------------------------------------
namespace {
auto test_stopped_as_optional() -> void {
    test_std::sender auto sndr1 = test_std::just(42) | test_std::stopped_as_optional;
    using sigs_of_sndr1         = test_std::completion_signatures_of_t<decltype(sndr1), test_detail::sync_wait_env>;
    static_assert(
        std::same_as<sigs_of_sndr1, test_std::completion_signatures<test_std::set_value_t(std::optional<int>)>>);
    auto [i] = test_std::sync_wait(std::move(sndr1)).value();
    ASSERT(i == 42);

    test_std::sender auto sndr2 = test_std::stopped_as_optional(test::optional_sender<int>{});
    using sigs_of_sndr2 = test_std::completion_signatures_of_t<decltype(sndr2), test_std::detail::sync_wait_env>;
    static_assert(
        std::same_as<sigs_of_sndr2, test_std::completion_signatures<test_std::set_value_t(std::optional<int>)>>);
    auto [opt] = test_std::sync_wait(std::move(sndr2)).value();
    ASSERT(!opt.has_value());
}

auto test_stopped_as_optional_attributes() {
    test::sender_env s{42};
    test::test_sender_env<true>(42, test::test_forwardable_attr{}, s);
    test::test_sender_env<true>(84, test::test_non_forwardable_attr{}, s);
    test::test_sender_env<true>(42, test::test_forwardable_attr{}, test_std::stopped_as_optional(s));
    test::test_sender_env<false>(84, test::test_non_forwardable_attr{}, test_std::stopped_as_optional(s));
}

struct stoppable_env {
    test_std::inplace_stop_token token;
    auto                         query(test_std::get_stop_token_t) const noexcept { return token; }
};

struct optional_receiver {
    using receiver_concept = test_std::receiver_tag;
    stoppable_env                      env;
    std::optional<std::optional<int>>* result;

    auto get_env() const noexcept { return env; }
    auto set_value(std::optional<int> value) && noexcept -> void { result->emplace(std::move(value)); }
    auto set_error(auto&&) && noexcept -> void { ASSERT_UNREACHABLE(); }
    auto set_stopped() && noexcept -> void { ASSERT_UNREACHABLE(); }
};

template <typename Sender, typename... Env>
concept has_value_scheduler = requires(const Sender& sender, const Env&... env) {
    test_std::get_completion_scheduler<test_std::set_value_t>(test_std::get_env(sender), env...);
};

auto test_stopped_as_optional_on_run_loop() -> void {
    test_std::run_loop            loop;
    test_std::inplace_stop_source active, stopped;
    const auto                    scheduler           = loop.get_scheduler();
    auto                          scheduled           = test_std::schedule(scheduler);
    const auto                    attrs               = test_std::get_env(scheduled);
    const auto                    get_value_scheduler = test_std::get_completion_scheduler<test_std::set_value_t>;
    ASSERT(get_value_scheduler(attrs) == scheduler);
    ASSERT(test_std::get_completion_scheduler<test_std::set_stopped_t>(attrs) == scheduler);

    auto sndr = scheduled | test_std::then([]() noexcept { return 42; }) | test_std::stopped_as_optional;
    static_assert(has_value_scheduler<decltype(sndr), test_std::env<>>);
    ASSERT(get_value_scheduler(test_std::get_env(sndr), test_std::env<>{}) == scheduler);
    // Without an unstoppable environment, both upstream tags can produce value. The mapper
    // cannot infer a shared origin from the two separate scheduler queries on the run loop.
    static_assert(!has_value_scheduler<decltype(sndr), stoppable_env>);
    static_assert(!has_value_scheduler<decltype(sndr)>);

    std::optional<std::optional<int>> active_result, stopped_result;
    auto active_op  = test_std::connect(sndr, optional_receiver{{active.get_token()}, &active_result});
    auto stopped_op = test_std::connect(std::move(sndr), optional_receiver{{stopped.get_token()}, &stopped_result});
    test_std::start(active_op);
    test_std::start(stopped_op);
    ASSERT(!active_result && !stopped_result);
    stopped.request_stop();
    loop.finish();
    loop.run();
    ASSERT(active_result && *active_result == 42);
    ASSERT(stopped_result && !stopped_result->has_value());
}

} // namespace
// ----------------------------------------------------------------------------

TEST(exec_stopped_as_optional) {
    test_stopped_as_optional();
    test_stopped_as_optional_attributes();
    test_stopped_as_optional_on_run_loop();
}
