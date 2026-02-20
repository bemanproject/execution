// tests/beman/execution/exec-on.test.cpp                           -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <concepts>
#include <test/thread_pool.hpp>
#include <test/execution.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
import beman.execution.detail;
#else
#include <beman/execution/detail/get_completion_signatures.hpp>
#include <beman/execution/detail/just.hpp>
#include <beman/execution/detail/make_sender.hpp>
#include <beman/execution/detail/on.hpp>
#include <beman/execution/detail/product_type.hpp>
#include <beman/execution/detail/sender_adaptor_closure.hpp>
#include <beman/execution/detail/sender_for.hpp>
#include <beman/execution/detail/sync_wait.hpp>
#include <beman/execution/detail/then.hpp>

#include <beman/execution/detail/suppress_push.hpp>
#endif

// ----------------------------------------------------------------------------

namespace {
template <test_std::scheduler Sch, test_std::sender Sndr, test_detail::is_sender_adaptor_closure Closure>
auto test_interface(Sch sch, Sndr sndr, Closure closure) -> void {
    static_assert(requires {
        { test_std::on(sch, sndr) } -> test_std::sender;
    });
    static_assert(requires {
        { test_std::on(sndr, sch, closure) } -> test_std::sender;
    });
    static_assert(requires {
        { test_std::on(sch, closure) } -> test_detail::is_sender_adaptor_closure;
    });

    auto sndr1{test_std::on(sch, sndr)};
    auto sndr2{test_std::on(sndr, sch, closure)};
    auto sndr3{test_std::on(sch, closure)};
    test::use(sndr1, sndr2, sndr3);
}

template <test_detail::sender_for<test_std::on_t> OutSndr>
auto test_transform_env(OutSndr out_sndr) -> void {
    auto e{test_std::on.transform_env(out_sndr, test_std::env<>{})};
    test::use(e);
}

template <test_detail::sender_for<test_std::on_t> OutSndr>
auto test_transform_sender(OutSndr out_sndr) -> void {
    auto s{test_std::on.transform_sender(std::move(out_sndr), test_std::env<>{})};
    static_assert(test_std::sender<decltype(s)>);
    auto ts{std::move(s) | test_std::then([](auto&&...) {})};
    static_assert(test_std::sender<decltype(ts)>);
}

struct on_receiver {
    using receiver_concept = test_std::receiver_t;
    test::thread_pool& pool;
    auto               set_value(auto&&...) && noexcept {}
    auto               set_error(auto&&) && noexcept {}
    auto               set_stopped() && noexcept {}
    auto get_env() const noexcept { return test_detail::make_env(test_std::get_scheduler, pool.get_scheduler()); }
};
static_assert(test_std::receiver<on_receiver>);
} // namespace

TEST(exec_on) {
    test::thread_pool pool{};

    static_assert(std::same_as<const test_std::on_t, decltype(test_std::on)>);
    static_assert(test_detail::is_sender_adaptor_closure<decltype(test_std::then([] {}))>);
    static_assert(not test_detail::is_sender_adaptor_closure<decltype(test_std::just([] {}))>);
    test_interface(pool.get_scheduler(), test_std::just(), test_std::then([] {}));

    test_transform_env(test_detail::make_sender(test_std::on, pool.get_scheduler(), test_std::just()));
    test_transform_env(test_detail::make_sender(
        test_std::on, test_detail::product_type{pool.get_scheduler(), test_std::then([] {})}, test_std::just()));

    test_transform_sender(test_detail::make_sender(test_std::on, pool.get_scheduler(), test_std::just()));
    test_transform_sender(test_detail::make_sender(
        test_std::on, test_detail::product_type{pool.get_scheduler(), test_std::then([] {})}, test_std::just()));

    std::thread::id on_id{};
    std::thread::id pool_id{};
    std::thread::id cont_id{};

    test_std::sync_wait(test_std::starts_on(pool.get_scheduler(), test_std::just() | test_std::then([&pool_id] {
                                                                      pool_id = std::this_thread::get_id();
                                                                  })));
    test_std::sync_wait(test_std::on(pool.get_scheduler(), test_std::just() | test_std::then([&on_id] {
                                                               on_id = std::this_thread::get_id();
                                                               return 42;
                                                           })) |
                        test_std::then([&cont_id](int val) {
                            assert(val == 42);
                            cont_id = std::this_thread::get_id();
                        }));
    assert(on_id == pool_id);
    assert(cont_id == std::this_thread::get_id());
    assert(on_id != std::this_thread::get_id());

    test_std::sync_wait(test_std::on(test_std::just(17), pool.get_scheduler(), test_std::then([&on_id](int val) {
                                         assert(val == 17);
                                         on_id = std::this_thread::get_id();
                                         return 42;
                                     })) |
                        test_std::then([&cont_id](int val) {
                            assert(val == 42);
                            cont_id = std::this_thread::get_id();
                        }));
    assert(on_id == pool_id);
    assert(cont_id == std::this_thread::get_id());
    assert(on_id != std::this_thread::get_id());

    test_std::sync_wait(test_std::just(17) | test_std::on(pool.get_scheduler(), test_std::then([&on_id](int val) {
                                                              assert(val == 17);
                                                              on_id = std::this_thread::get_id();
                                                              return 42;
                                                          })) |
                        test_std::then([&cont_id](int val) {
                            assert(val == 42);
                            cont_id = std::this_thread::get_id();
                        }));
    assert(on_id == pool_id);
    assert(cont_id == std::this_thread::get_id());
    assert(on_id != std::this_thread::get_id());
}
