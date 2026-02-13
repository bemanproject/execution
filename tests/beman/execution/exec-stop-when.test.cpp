// tests/beman/execution/exec-stop-when.test.cpp                      -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <concepts>
#include <optional>
#include <type_traits>
#include <utility>
#include <test/execution.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
import beman.execution.detail;
#else
#include <beman/execution/detail/stop_when.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/completion_signatures.hpp>
#include <beman/execution/detail/set_value.hpp>
#include <beman/execution/detail/set_stopped.hpp>
#include <beman/execution/detail/receiver.hpp>
#include <beman/execution/detail/operation_state.hpp>
#include <beman/execution/detail/inplace_stop_source.hpp>
#include <beman/execution/detail/get_stop_token.hpp>
#include <beman/execution/detail/get_env.hpp>
#include <beman/execution/detail/connect.hpp>
#include <beman/execution/detail/start.hpp>
#include <beman/execution/detail/stop_callback_for_t.hpp>
#include <beman/execution/detail/never_stop_token.hpp>
#include <beman/execution/detail/just.hpp>
#include <beman/execution/detail/sync_wait.hpp>
#endif

// ----------------------------------------------------------------------------

namespace {
struct sender {
    using sender_concept        = test_std::sender_t;
    using completion_signatures = test_std::completion_signatures<test_std::set_value_t(), test_std::set_stopped_t()>;

    template <test_std::receiver Rcvr>
    struct state {
        using operation_state_concept = test_std::operation_state_t;
        using rcvr_t                  = std::remove_cvref_t<Rcvr>;
        using token_t = decltype(test_std::get_stop_token(test_std::get_env(std::declval<const rcvr_t>())));

        struct cb_t {
            rcvr_t& rcvr;
            auto    operator()() noexcept { test_std::set_stopped(std::move(this->rcvr)); }
        };
        using stop_cb_t = test_std::stop_callback_for_t<token_t, cb_t>;

        rcvr_t                   rcvr;
        std::optional<stop_cb_t> stop_cb;

        template <test_std::receiver R>
        state(R&& r) : rcvr(r) {}

        auto start() & noexcept {
            this->stop_cb.emplace(test_std::get_stop_token(test_std::get_env(this->rcvr)), cb_t{this->rcvr});
        }
    };

    template <test_std::receiver Rcvr>
    auto connect(Rcvr&& rcvr) -> state<Rcvr> {
        return state<Rcvr>(std::forward<Rcvr>(rcvr));
    }
};
static_assert(test_std::sender<sender>);

struct env {
    test_std::inplace_stop_token token;
    auto                         query(const test_std::get_stop_token_t&) const noexcept { return this->token; }
};
enum class completion : char { none, value, stopped };

struct receiver {
    using receiver_concept = test_std::receiver_t;

    test_std::inplace_stop_token token;
    completion&                  comp;

    auto set_value() && noexcept { this->comp = completion::value; }
    auto set_stopped() && noexcept { this->comp = completion::stopped; }

    auto get_env() const noexcept { return env{this->token}; }
};
static_assert(test_std::receiver<receiver>);
} // namespace

TEST(exec_stop_when) {
    static_assert(std::same_as<const test_detail::stop_when_t, decltype(test_detail::stop_when)>);
    static_assert(requires(const env& e) {
        { test_std::get_stop_token(e) } -> std::same_as<test_std::inplace_stop_token>;
    });
    static_assert(requires(const receiver& r) {
        { test_std::get_stop_token(test_std::get_env(r)) } -> std::same_as<test_std::inplace_stop_token>;
    });

    {
        test_std::inplace_stop_source source;
        completion                    comp{completion::none};
        auto                          state{test_std::connect(sender{}, receiver{source.get_token(), comp})};
        ASSERT(comp == completion::none);
        test_std::start(state);
        ASSERT(comp == completion::none);

        source.request_stop();
        ASSERT(comp == completion::stopped);
    }
    {
        auto sndr{test_detail::stop_when(sender{}, test_std::never_stop_token{})};
        static_assert(std::same_as<sender, decltype(sndr)>);
    }
    {
        test_std::inplace_stop_source source;
        completion                    comp{completion::none};
        auto state{test_std::connect(test_detail::stop_when(sender{}, test_std::never_stop_token{}),
                                     receiver{source.get_token(), comp})};
        ASSERT(comp == completion::none);
        test_std::start(state);
        ASSERT(comp == completion::none);

        source.request_stop();
        ASSERT(comp == completion::stopped);
    }
    {
        test_std::inplace_stop_source source1;
        test_std::inplace_stop_source source2;
        completion                    comp{completion::none};
        auto                          state{test_std::connect(test_detail::stop_when(sender{}, source1.get_token()),
                                     receiver{source2.get_token(), comp})};
        ASSERT(comp == completion::none);
        test_std::start(state);
        ASSERT(comp == completion::none);

        source1.request_stop();
        ASSERT(comp == completion::stopped);
    }
    {
        test_std::inplace_stop_source source1;
        test_std::inplace_stop_source source2;
        completion                    comp{completion::none};
        auto                          state{test_std::connect(test_detail::stop_when(sender{}, source1.get_token()),
                                     receiver{source2.get_token(), comp})};
        ASSERT(comp == completion::none);
        test_std::start(state);
        ASSERT(comp == completion::none);

        source2.request_stop();
        ASSERT(comp == completion::stopped);
    }
    test_std::inplace_stop_source source;
    test_std::sync_wait(test_detail::stop_when(test_std::just(), source.get_token()));
}
