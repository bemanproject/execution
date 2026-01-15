// tests/beman/execution/exec-affine-on.test.cpp                      -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/detail/affine_on.hpp>
#include <beman/execution/detail/connect.hpp>
#include <beman/execution/detail/env.hpp>
#include <beman/execution/detail/get_completion_scheduler.hpp>
#include <beman/execution/detail/get_scheduler.hpp>
#include <beman/execution/detail/get_stop_token.hpp>
#include <beman/execution/detail/just.hpp>
#include <beman/execution/detail/prop.hpp>
#include <beman/execution/detail/read_env.hpp>
#include <beman/execution/detail/run_loop.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/sender_in.hpp>
#include <beman/execution/detail/starts_on.hpp>
#include <beman/execution/detail/sync_wait.hpp>
#include <beman/execution/detail/then.hpp>
#include <beman/execution/detail/when_all.hpp>
#include <beman/execution/detail/write_env.hpp>

#include <mutex>
#include <condition_variable>
#include <thread>

#include <test/execution.hpp>

// ----------------------------------------------------------------------------

namespace {
struct awaiter {
    bool                    done{false};
    std::mutex              mtx{};
    std::condition_variable cv{};

    auto complete() -> bool {
        std::lock_guard lock{this->mtx};
        this->done = true;
        this->cv.notify_all();
        return true;
    }
    auto await() {
        std::unique_lock lock{this->mtx};
        this->cv.wait(lock, [&]() noexcept { return this->done; });
    }
};
template <test_std::scheduler Sched>
struct receiver {
    using receiver_concept = test_std::receiver_t;
    Sched    scheduler_;
    awaiter* awaiter_{nullptr};
    auto     set_value(auto&&...) && noexcept -> void { this->awaiter_&& this->awaiter_->complete(); }
    auto     set_error(auto&&) && noexcept -> void { this->awaiter_&& this->awaiter_->complete(); }
    auto     set_stopped() && noexcept -> void { this->awaiter_&& this->awaiter_->complete(); }

    auto get_env() const noexcept { return test_std::env{test_std::prop(test_std::get_scheduler, this->scheduler_)}; }
};
template <test_std::scheduler Sched>
receiver(Sched, awaiter* = nullptr) -> receiver<Sched>;

struct test_scheduler {
    using scheduler_concept = test_std::scheduler_t;

    struct data {
        std::size_t connected_{};
        std::size_t started_{};
    };
    data* data_;

    template <test_std::receiver Receiver>
    struct state {
        using operation_state_concept = test_std::operation_state_t;
        std::remove_cvref_t<Receiver> receiver_;
        data*                         data_;
        auto                          start() & noexcept -> void {
            ++this->data_->started_;
            test_std::set_value(std::move(this->receiver_));
        }
    };
    struct sender {
        using sender_concept        = test_std::sender_t;
        using completion_signatures = test_std::completion_signatures<test_std::set_value_t()>;
        data* data_;
        struct env {
            data* data_;
            auto  query(test_std::get_completion_scheduler_t<test_std::set_value_t>) const noexcept {
                return test_scheduler{};
            }
        };
        auto get_env() const noexcept -> env { return {this->data_}; }
        template <test_std::receiver Receiver>
        auto connect(Receiver&& rcvr) && noexcept -> state<Receiver> {
            ++this->data_->connected_;
            return {std::forward<Receiver>(rcvr), this->data_};
        }
    };

    auto        schedule() const noexcept { return sender{this->data_}; }
    friend auto operator==(const test_scheduler&, const test_scheduler&) noexcept -> bool = default;
};

static_assert(test_std::scheduler<test_scheduler>);

auto test_order_of_connect() -> void {
    test_scheduler::data inner_data{};
    test_scheduler       inner_sched{&inner_data};

    test_scheduler::data data{};
    test_scheduler       sched{&data};
    auto                 sndr{test_std::affine_on(test_std::starts_on(inner_sched, test_std::just(42)))};

    assert(data.connected_ == 0);
    assert(data.started_ == 0);
    assert(inner_data.connected_ == 0);
    assert(inner_data.started_ == 0);
    auto st{test_std::connect(std::move(sndr), receiver{sched})};
    assert(data.connected_ == 1);
    assert(data.started_ == 0);
    assert(inner_data.connected_ == 1);
    assert(inner_data.started_ == 0);
    test_std::start(st);
    assert(data.connected_ == 1);
    assert(data.started_ == 1);
    assert(inner_data.connected_ == 1);
    assert(inner_data.started_ == 1);
}

template <test_std::sender Sender>
auto test_affine_on_specializations(Sender&& sender, std::size_t count = 0u) -> void {
    test_scheduler::data data{};
    test_scheduler       sched{&data};
    auto                 sndr{test_std::affine_on(std::forward<Sender>(sender))};
    awaiter              aw{};

    assert(data.connected_ == 0);
    assert(data.started_ == 0);
    auto st{test_std::connect(std::move(sndr), receiver{sched, &aw})};
    assert(data.connected_ == count);
    assert(data.started_ == 0);
    test_std::start(st);
    aw.await();

    assert(data.connected_ == count);
    assert(data.started_ == count);
}
} // namespace

auto main() -> int {
    static_assert(test_std::sender<decltype(test_std::affine_on(test_std::just(42)))>);
    static_assert(test_std::sender<decltype(test_std::just(42) | test_std::affine_on())>);

    static_assert(not test_std::sender_in<decltype(test_std::affine_on(test_std::just(42))), test_std::env<>>);

    test_std::run_loop loop;
    auto               r{receiver(loop.get_scheduler())};
    static_assert(test_std::receiver<decltype(r)>);
    auto s{test_std::get_scheduler(test_std::get_env(r))};
    assert(s == loop.get_scheduler());
    auto st{test_std::transform_sender(
        test_std::default_domain(), test_std::affine_on(test_std::just(42)), test_std::get_env(r))};
    test_std::connect(std::move(st), std::move(r));
    auto s0{test_std::connect(test_std::affine_on(test_std::just(42)), receiver(loop.get_scheduler()))};

    std::thread t{[&]() noexcept { loop.run(); }};
    auto        r0 = test_std::sync_wait(test_std::affine_on(test_std::just(42)));
    assert(r0);
    auto [v0] = *r0;
    assert(v0 == 42);
    auto r1 = test_std::sync_wait(test_std::starts_on(loop.get_scheduler(), test_std::affine_on(test_std::just(42))));
    assert(r1);
    auto [v1] = *r1;
    assert(v1 == 42);

    test_order_of_connect();
    test_affine_on_specializations(test_std::just(42));
    test_affine_on_specializations(test_std::just(42, true, 3.14));
    test_affine_on_specializations(test_std::just_error(42));
    test_affine_on_specializations(test_std::just_stopped());
    test_affine_on_specializations(test_std::just_stopped());
    test_affine_on_specializations(test_std::read_env(test_std::get_stop_token));
    test_affine_on_specializations(test_std::write_env(
        test_std::just(42), test_std::env{test_std::prop{test_std::get_stop_token, test_std::never_stop_token{}}}));
    test_affine_on_specializations(
        test_std::write_env(test_std::starts_on(loop.get_scheduler(), test_std::just(42)),
                            test_std::env{test_std::prop{test_std::get_stop_token, test_std::never_stop_token{}}}),
        1u);
    test_affine_on_specializations(test_std::then(test_std::just(42), [](int) {}));
    test_affine_on_specializations(
        test_std::then(test_std::starts_on(loop.get_scheduler(), test_std::just(42)), [](auto&&...) {}), 1u);
    test_affine_on_specializations(test_std::upon_error(test_std::just_error(42), [](int) {}));
    test_affine_on_specializations(
        test_std::upon_error(test_std::starts_on(loop.get_scheduler(), test_std::just_error(42)), [](auto&&...) {}),
        1u);
    test_affine_on_specializations(test_std::upon_stopped(test_std::just_stopped(), [] {}));
    test_affine_on_specializations(
        test_std::upon_stopped(test_std::starts_on(loop.get_scheduler(), test_std::just_stopped()), [] {}), 1u);

    loop.finish();
    t.join();

    return 0;
}
