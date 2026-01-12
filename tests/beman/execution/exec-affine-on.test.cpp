// tests/beman/execution/exec-affine-on.test.cpp                      -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/detail/affine_on.hpp>
#include <beman/execution/detail/sync_wait.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/sender_in.hpp>
#include <beman/execution/detail/connect.hpp>
#include <beman/execution/detail/just.hpp>
#include <beman/execution/detail/env.hpp>
#include <beman/execution/detail/prop.hpp>
#include <beman/execution/detail/get_scheduler.hpp>
#include <beman/execution/detail/run_loop.hpp>
#include <test/execution.hpp>

// ----------------------------------------------------------------------------

namespace {
template <test_std::scheduler Sched>
struct receiver {
    using receiver_concept = test_std::receiver_t;
    Sched scheduler_;
    auto  set_value(auto&&...) && noexcept -> void {}
    auto  set_error(auto&&) && noexcept -> void {}
    auto  set_stopped() && noexcept -> void {}

    auto get_env() const noexcept { return test_std::env{test_std::prop(test_std::get_scheduler, this->scheduler_)}; }
};
template <test_std::scheduler Sched>
receiver(Sched) -> receiver<Sched>;
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
    return 0;
}
