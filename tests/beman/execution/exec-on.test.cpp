// tests/beman/execution/exec-on.test.cpp                           -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <test/execution.hpp>
#include <test/thread_pool.hpp>
#include <test/sender_env.hpp>
#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <concepts>
#endif
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
struct custom_domain : test_std::default_domain {};

struct custom_scheduler {
    using scheduler_concept = test_std::scheduler_tag;

    static auto query(test_std::get_forward_progress_guarantee_t) noexcept {
        return test_std::forward_progress_guarantee::weakly_parallel;
    }

    static auto query(test_std::get_completion_scheduler_t<test_std::set_value_t>) noexcept -> custom_scheduler {
        return {};
    }

    static auto query(test_std::get_completion_domain_t<test_std::set_value_t>) noexcept -> custom_domain {
        return {};
    }

    struct env {
        static auto query(test_std::get_completion_scheduler_t<test_std::set_value_t>) noexcept -> custom_scheduler {
            return {};
        }
    };

    struct sender {
        using sender_concept = test_std::sender_tag;

        template <typename...>
        static consteval auto get_completion_signatures() noexcept {
            return test_std::completion_signatures<test_std::set_value_t()>{};
        }

        auto connect(auto rcvr) const noexcept { return test_std::connect(test_std::just(), std::move(rcvr)); }

        auto get_env() const noexcept -> env { return {}; }
    };

    static auto schedule() noexcept { return sender{}; }

    auto operator==(const custom_scheduler&) const -> bool = default;
};

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

struct on_receiver {
    using receiver_concept = test_std::receiver_tag;
    test::thread_pool& pool;
    auto               set_value(auto&&...) && noexcept {}
    auto               set_error(auto&&) && noexcept {}
    auto               set_stopped() && noexcept {}
    auto get_env() const noexcept { return test_detail::make_env(test_std::get_scheduler, pool.get_scheduler()); }
};
static_assert(test_std::receiver<on_receiver>);

auto test_on_attributes() {
    test_std::run_loop loop{};
    static_assert(test_std::dependent_sender<decltype(test_std::on(loop.get_scheduler(), test_std::just()))>);

    test::sender_env s{42};
    auto             recv_env = test_detail::make_env(test_std::get_start_scheduler, custom_scheduler{});

    test::test_sender_env<true>(42, test::test_forwardable_attr{}, s);
    test::test_sender_env<true>(84, test::test_non_forwardable_attr{}, s);
    test::test_sender_env<true>(42, test::test_forwardable_attr{}, test_std::on(loop.get_scheduler(), s));
    test::test_sender_env<false>(84, test::test_non_forwardable_attr{}, test_std::on(loop.get_scheduler(), s));
    test::test_sender_env<true>(
        42, test::test_forwardable_attr{}, test_std::on(s, loop.get_scheduler(), test_std::then([]() {})));
    test::test_sender_env<false>(
        84, test::test_non_forwardable_attr{}, test_std::on(s, loop.get_scheduler(), test_std::then([]() {})));

    auto on_sndr1 = test_std::on(loop.get_scheduler(), test_std::just());
    static_assert(test_std::sender_in<decltype(on_sndr1), decltype(recv_env)>);
    static_assert(not test_std::sender_in<decltype(on_sndr1), test_std::env<>>);

    auto attrs1 = test_std::get_env(on_sndr1);

    ASSERT(test_std::get_completion_scheduler<test_std::set_value_t>(attrs1, recv_env) == custom_scheduler{});

    static_assert(std::same_as<decltype(test_std::get_completion_domain<test_std::set_value_t>(attrs1, recv_env)),
                               custom_domain>);

    auto attrs2 = test_std::get_env(
        test_std::on(test_std::schedule(custom_scheduler{}), loop.get_scheduler(), test_std::then([]() {})));
    ASSERT(test_std::get_completion_scheduler<test_std::set_value_t>(attrs2, test_std::env<>{}) == custom_scheduler{});
    static_assert(
        std::same_as<decltype(test_std::get_completion_domain<test_std::set_value_t>(attrs2, test_std::env<>{})),
                     custom_domain>);
}
} // namespace

TEST(exec_on) {
    test::thread_pool pool{};

    static_assert(std::same_as<const test_std::on_t, decltype(test_std::on)>);
    static_assert(test_detail::is_sender_adaptor_closure<decltype(test_std::then([] {}))>);
    static_assert(not test_detail::is_sender_adaptor_closure<decltype(test_std::just([] {}))>);
    test_interface(pool.get_scheduler(), test_std::just(), test_std::then([] {}));

    std::thread::id on_id{};
    std::thread::id pool_id{};
    std::thread::id cont_id{};

    test_std::sync_wait(test_std::starts_on(pool.get_scheduler(), test_std::just() | test_std::then([&pool_id] {
                                                                      pool_id = std::this_thread::get_id();
                                                                  })));
    test_std::sync_wait(test_std::on(pool.get_scheduler(), test_std::just()));
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

#if 0
//-dk:TODO
    test_std::sync_wait(test_std::on(test_std::just(17), pool.get_scheduler(), test_std::then([&on_id](int val) {
                                         assert(val == 17);
                                         on_id = std::this_thread::get_id();
                                         return 42;
                                     })) |
                        test_std::then([&cont_id](int val) {
                            assert(val == 42);
                            cont_id = std::this_thread::get_id();
                        }));
#endif
    assert(on_id == pool_id);
    assert(cont_id == std::this_thread::get_id());
    assert(on_id != std::this_thread::get_id());

#if 0
    //-dk:TODO
    test_std::sync_wait(test_std::just(17) | test_std::on(pool.get_scheduler(), test_std::then([&on_id](int val) {
                                                              assert(val == 17);
                                                              on_id = std::this_thread::get_id();
                                                              return 42;
                                                          })) |
                        test_std::then([&cont_id](int val) {
                            assert(val == 42);
                            cont_id = std::this_thread::get_id();
                        }));
#endif
    assert(on_id == pool_id);
    assert(cont_id == std::this_thread::get_id());
    assert(on_id != std::this_thread::get_id());

    test_on_attributes();
}
