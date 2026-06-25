// src/beman/execution/tests/exec-bulk.test.cpp -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <version>
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <numeric>
#include <ranges>
#include <stdexcept>
#include <tuple>
#include <vector>
#ifdef __cpp_lib_parallel_algorithm
#include <execution>
#endif
#include <test/execution.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
import beman.execution.detail.meta.combine;
import beman.execution.detail.meta.unique;
#else
#include <beman/execution/execution.hpp>
#endif

namespace {

auto test_bulk() {
    auto b0 = test_std::bulk(test_std::just(), test_std::seq, 1, [](int) {});

    static_assert(test_std::sender<decltype(b0)>);
    auto b0_env         = test_std::get_env(b0);
    auto b0_completions = test_std::get_completion_signatures<decltype(b0), decltype(b0_env)>();
    static_assert(
        std::is_same_v<
            decltype(b0_completions),
            test_std::completion_signatures<test_std::set_value_t(), test_std::set_error_t(std::exception_ptr)>>,
        "Completion signatures do not match!");

    int counter = 0;

    auto b1 = test_std::bulk(test_std::just(), test_std::seq, 5, [&](int i) { counter += i; });

    static_assert(test_std::sender<decltype(b1)>);
    auto b1_env         = test_std::get_env(b0);
    auto b1_completions = test_std::get_completion_signatures<decltype(b1), decltype(b1_env)>();
    static_assert(
        std::is_same_v<
            decltype(b1_completions),
            test_std::completion_signatures<test_std::set_value_t(), test_std::set_error_t(std::exception_ptr)>>,
        "Completion signatures do not match!");
    test_std::sync_wait(b1);
    ASSERT(counter == 10);

    std::vector<int> a{1, 2, 3, 4, 5, 6, 7, 8};
    std::vector<int> b{9, 10, 11, 13, 14, 15, 16, 17};

    std::vector<int> results(a.size(), 0);

    auto b2 = test_std::bulk(
        test_std::just(a), test_std::seq, a.size(), [&](std::size_t index, const std::vector<int>& vec) {
            results[index] = vec[index] * b[index];
        });
    static_assert(test_std::sender<decltype(b2)>);
    auto b2_env         = test_std::get_env(b2);
    auto b2_completions = test_std::get_completion_signatures<decltype(b2), decltype(b2_env)>();
    static_assert(std::is_same_v<decltype(b2_completions),
                                 test_std::completion_signatures<test_std::set_value_t(std::vector<int>),
                                                                 test_std::set_error_t(std::exception_ptr)>>,
                  "Completion signatures do not match!");
    test_std::sync_wait(b2);

    std::vector<int> expected{9, 20, 33, 52, 70, 90, 112, 136};

    for (::std::size_t i = 0; i < results.size(); ++i) {
        ASSERT(results[i] == expected[i]);
    }
}

auto test_bulk_noexcept() {
    auto b0             = test_std::bulk(test_std::just(), test_std::seq, 1, [](int) noexcept {});
    auto b0_env         = test_std::get_env(b0);
    auto b0_completions = test_std::get_completion_signatures<decltype(b0), decltype(b0_env)>();
    static_assert(std::is_same_v<decltype(b0_completions), test_std::completion_signatures<test_std::set_value_t()>>,
                  "Completion signatures do not match!");
    static_assert(test_std::sender<decltype(b0)>);

    int counter = 0;

    auto b1 = test_std::bulk(test_std::just(), test_std::seq, 5, [&](int i) noexcept { counter += i; });

    static_assert(test_std::sender<decltype(b1)>);
    auto b1_env         = test_std::get_env(b0);
    auto b1_completions = test_std::get_completion_signatures<decltype(b1), decltype(b1_env)>();
    static_assert(std::is_same_v<decltype(b1_completions), test_std::completion_signatures<test_std::set_value_t()>>,
                  "Completion signatures do not match!");
    test_std::sync_wait(b1);
    ASSERT(counter == 10);
}

auto test_bulk_pipeable() {
    auto b0 = test_std::just() | test_std::bulk(test_std::seq, 1, [](int) {});

    static_assert(test_std::sender<decltype(b0)>);
    auto b0_env         = test_std::get_env(b0);
    auto b0_completions = test_std::get_completion_signatures<decltype(b0), decltype(b0_env)>();
    static_assert(
        std::is_same_v<
            decltype(b0_completions),
            test_std::completion_signatures<test_std::set_value_t(), test_std::set_error_t(std::exception_ptr)>>,
        "Completion signatures do not match!");

    int counter = 0;

    auto b1 = test_std::just() | test_std::bulk(test_std::seq, 5, [&](int i) { counter += i; });

    static_assert(test_std::sender<decltype(b1)>);
    auto b1_env         = test_std::get_env(b0);
    auto b1_completions = test_std::get_completion_signatures<decltype(b1), decltype(b1_env)>();
    static_assert(
        std::is_same_v<
            decltype(b1_completions),
            test_std::completion_signatures<test_std::set_value_t(), test_std::set_error_t(std::exception_ptr)>>,
        "Completion signatures do not match!");
    test_std::sync_wait(b1);
    ASSERT(counter == 10);

    std::vector<int> a{1, 2, 3, 4, 5, 6, 7, 8};
    std::vector<int> b{9, 10, 11, 13, 14, 15, 16, 17};

    std::vector<int> results(a.size(), 0);

    auto b2 = test_std::just(a) |
              test_std::bulk(test_std::seq, a.size(), [&](std::size_t index, const std::vector<int>& vec) {
                  results[index] = vec[index] * b[index];
              });

    static_assert(test_std::sender<decltype(b2)>);
    auto b2_env         = test_std::get_env(b2);
    auto b2_completions = test_std::get_completion_signatures<decltype(b2), decltype(b2_env)>();
    static_assert(std::is_same_v<decltype(b2_completions),
                                 test_std::completion_signatures<test_std::set_value_t(std::vector<int>),
                                                                 test_std::set_error_t(std::exception_ptr)>>,
                  "Completion signatures do not match!");
    test_std::sync_wait(b2);

    std::vector<int> expected{9, 20, 33, 52, 70, 90, 112, 136};

    for (::std::size_t i = 0; i < results.size(); ++i) {
        ASSERT(results[i] == expected[i]);
    }
}

auto test_bulk_chunked() {
    int counter = 0;

    auto b0 = test_std::bulk_chunked(test_std::just(), test_std::seq, 5, [&](int begin, int end) {
        for (int i = begin; i < end; ++i) {
            counter += i;
        }
    });

    static_assert(test_std::sender<decltype(b0)>);
    test_std::sync_wait(b0);
    ASSERT(counter == 10);
}

auto test_bulk_chunked_with_values() {
    std::vector<int> a{1, 2, 3, 4};
    std::vector<int> results(a.size(), 0);

    auto b0 = test_std::bulk_chunked(test_std::just(a),
                                     test_std::seq,
                                     a.size(),
                                     [&](std::size_t begin, std::size_t end, const std::vector<int>& vec) {
                                         for (std::size_t i = begin; i < end; ++i) {
                                             results[i] = vec[i] * 2;
                                         }
                                     });

    static_assert(test_std::sender<decltype(b0)>);
    test_std::sync_wait(b0);

    std::vector<int> expected{2, 4, 6, 8};
    for (std::size_t i = 0; i < results.size(); ++i) {
        ASSERT(results[i] == expected[i]);
    }
}

auto test_bulk_chunked_pipeable() {
    int counter = 0;

    auto b0 = test_std::just() | test_std::bulk_chunked(test_std::seq, 5, [&](int begin, int end) {
                  for (int i = begin; i < end; ++i) {
                      counter += i;
                  }
              });

    static_assert(test_std::sender<decltype(b0)>);
    test_std::sync_wait(b0);
    ASSERT(counter == 10);
}

auto test_bulk_unchunked() {
    int counter = 0;

    auto b0 = test_std::bulk_unchunked(test_std::just(), test_std::seq, 5, [&](int i) { counter += i; });

    static_assert(test_std::sender<decltype(b0)>);
    test_std::sync_wait(b0);
    ASSERT(counter == 10);
}

auto test_bulk_unchunked_with_values() {
    std::vector<int> a{1, 2, 3, 4};
    std::vector<int> results(a.size(), 0);

    auto b0 = test_std::bulk_unchunked(
        test_std::just(a), test_std::seq, a.size(), [&](std::size_t index, const std::vector<int>& vec) {
            results[index] = vec[index] * 3;
        });

    static_assert(test_std::sender<decltype(b0)>);
    test_std::sync_wait(b0);

    std::vector<int> expected{3, 6, 9, 12};
    for (std::size_t i = 0; i < results.size(); ++i) {
        ASSERT(results[i] == expected[i]);
    }
}

auto test_bulk_unchunked_pipeable() {
    int counter = 0;

    auto b0 = test_std::just() | test_std::bulk_unchunked(test_std::seq, 5, [&](int i) { counter += i; });

    static_assert(test_std::sender<decltype(b0)>);
    test_std::sync_wait(b0);
    ASSERT(counter == 10);
}

auto test_execution_policies() {
    int counter = 0;
    test_std::sync_wait(test_std::bulk(test_std::just(), test_std::par, 5, [&](int i) { counter += i; }));
    ASSERT(counter == 10);

    counter = 0;
    test_std::sync_wait(test_std::bulk(test_std::just(), test_std::par_unseq, 5, [&](int i) { counter += i; }));
    ASSERT(counter == 10);

    counter = 0;
    test_std::sync_wait(test_std::bulk(test_std::just(), test_std::unseq, 5, [&](int i) { counter += i; }));
    ASSERT(counter == 10);
}

auto test_bulk_shape_zero() {
    int counter = 0;
    test_std::sync_wait(test_std::bulk(test_std::just(), test_std::seq, 0, [&](int) { counter++; }));
    ASSERT(counter == 0);

    test_std::sync_wait(test_std::bulk_chunked(test_std::just(), test_std::seq, 0, [&](int, int) { counter++; }));
    ASSERT(counter == 0);

    test_std::sync_wait(test_std::bulk_unchunked(test_std::just(), test_std::seq, 0, [&](int) { counter++; }));
    ASSERT(counter == 0);
}

auto test_bulk_exception_handling() {
    bool error_caught = false;

    auto sndr = test_std::bulk(test_std::just(), test_std::seq, 5, [](int i) {
        if (i == 3)
            throw std::runtime_error("test error");
    });

    try {
        test_std::sync_wait(sndr);
    } catch (...) {
        error_caught = true;
    }
    ASSERT(error_caught);
}

auto test_bulk_chunked_exception_handling() {
    bool error_caught = false;

    auto sndr = test_std::bulk_chunked(
        test_std::just(), test_std::seq, 5, [](int, int) { throw std::runtime_error("chunked error"); });

    try {
        test_std::sync_wait(sndr);
    } catch (...) {
        error_caught = true;
    }
    ASSERT(error_caught);
}

auto test_bulk_unchunked_exception_handling() {
    bool error_caught = false;

    auto sndr = test_std::bulk_unchunked(test_std::just(), test_std::seq, 5, [](int i) {
        if (i == 2)
            throw std::runtime_error("unchunked error");
    });

    try {
        test_std::sync_wait(sndr);
    } catch (...) {
        error_caught = true;
    }
    ASSERT(error_caught);
}

auto test_bulk_chunked_noexcept() {
    auto b0             = test_std::bulk_chunked(test_std::just(), test_std::seq, 1, [](int, int) noexcept {});
    auto b0_env         = test_std::get_env(b0);
    auto b0_completions = test_std::get_completion_signatures<decltype(b0), decltype(b0_env)>();
    static_assert(std::is_same_v<decltype(b0_completions), test_std::completion_signatures<test_std::set_value_t()>>,
                  "Chunked noexcept completion signatures do not match!");
}

auto test_bulk_unchunked_noexcept() {
    auto b0             = test_std::bulk_unchunked(test_std::just(), test_std::seq, 1, [](int) noexcept {});
    auto b0_env         = test_std::get_env(b0);
    auto b0_completions = test_std::get_completion_signatures<decltype(b0), decltype(b0_env)>();
    static_assert(std::is_same_v<decltype(b0_completions), test_std::completion_signatures<test_std::set_value_t()>>,
                  "Unchunked noexcept completion signatures do not match!");
}

auto test_bulk_shape_one() {
    int counter = 0;
    test_std::sync_wait(test_std::bulk(test_std::just(), test_std::seq, 1, [&](int i) {
        ASSERT(i == 0);
        counter++;
    }));
    ASSERT(counter == 1);

    counter = 0;
    test_std::sync_wait(test_std::bulk_chunked(test_std::just(), test_std::seq, 1, [&](int begin, int end) {
        ASSERT(begin == 0);
        ASSERT(end == 1);
        counter++;
    }));
    ASSERT(counter == 1);

    counter = 0;
    test_std::sync_wait(test_std::bulk_unchunked(test_std::just(), test_std::seq, 1, [&](int i) {
        ASSERT(i == 0);
        counter++;
    }));
    ASSERT(counter == 1);
}

auto test_bulk_chunked_covers_full_range() {
    std::size_t seen_begin = 999;
    std::size_t seen_end   = 999;
    int         call_count = 0;

    test_std::sync_wait(test_std::bulk_chunked(
        test_std::just(), test_std::seq, std::size_t(10), [&](std::size_t begin, std::size_t end) {
            seen_begin = begin;
            seen_end   = end;
            call_count++;
        }));

    ASSERT(call_count == 1);
    ASSERT(seen_begin == 0);
    ASSERT(seen_end == 10);
}

auto test_bulk_multiple_values() {
    int sum_a = 0;
    int sum_b = 0;

    test_std::sync_wait(test_std::bulk(test_std::just(10, 20), test_std::seq, 3, [&](int i, int a, int b) {
        sum_a += a;
        sum_b += b + i;
    }));

    ASSERT(sum_a == 30);
    ASSERT(sum_b == 60 + 0 + 1 + 2);
}
#ifdef __cpp_lib_parallel_algorithm
template <bool IsChunked, typename Policy, typename Shape, typename Fn, typename Child>
struct pstl_for_each_sender {
    using sender_concept = test_std::sender_tag;

    template <typename Rcvr>
    struct receiver {
        using receiver_concept = test_std::receiver_tag;

        template <typename... Args>
        auto set_value(Args&&... args) noexcept -> void {
            try {
                auto iota = std::views::iota(Shape(0), shape);
                std::for_each(policy, std::ranges::begin(iota), std::ranges::end(iota), [&](Shape i) {
                    if constexpr (IsChunked) {
                        std::invoke(fn, i, i + 1, args...);
                    } else {
                        std::invoke(fn, i, args...);
                    };
                });
                test_std::set_value(std::move(rcvr), std::forward<Args>(args)...);
            } catch (...) {
                test_std::set_error(std::move(rcvr), std::current_exception());
            }
        }

        template <typename E>
        auto set_error(E e) noexcept -> void {
            test_std::set_error(std::move(rcvr), std::move(e));
        }

        auto set_stopped() noexcept -> void { test_std::set_stopped(std::move(rcvr)); }

        auto get_env() const noexcept { return test_std::get_env(rcvr); }

        Rcvr   rcvr;
        Policy policy;
        Shape  shape;
        Fn     fn;
    };

    template <typename, typename... Env>
    static consteval auto get_completion_signatures() {
        constexpr auto compl_sigs = test_std::get_completion_signatures<Child, Env...>();
        return test_std::detail::meta::unique<test_std::detail::meta::combine<
            std::remove_cvref_t<decltype(compl_sigs)>,
            test_std::completion_signatures<test_std::set_error_t(std::exception_ptr)>>>{};
    }

    template <typename Rcvr>
    auto connect(Rcvr rcvr) && noexcept {
        return test_std::connect(child, receiver{std::move(rcvr), std::move(policy), std::move(shape), std::move(fn)});
    }

    auto get_env() const noexcept { return test_std::get_env(child); }

    Policy policy;
    Shape  shape;
    Fn     fn;
    Child  child;
};

template <typename Tag>
struct pstl_domain {
    template <typename Sndr, typename Env>
        requires std::same_as<test_std::tag_of_t<Sndr>, test_std::bulk_chunked_t> ||
                 std::same_as<test_std::tag_of_t<Sndr>, test_std::bulk_unchunked_t>
    static auto transform_sender(Tag, Sndr sndr, const Env&) {
        auto [_, data, child]    = std::move(sndr);
        auto [policy, shape, fn] = std::move(data);
        return pstl_for_each_sender<std::same_as<test_std::tag_of_t<Sndr>, test_std::bulk_chunked_t>,
                                    decltype(policy),
                                    decltype(shape),
                                    decltype(fn),
                                    decltype(child)>{
            std::move(policy), std::move(shape), std::move(fn), std::move(child)};
    }
};

struct pstl_env1 {
    static auto query(test_std::get_domain_t) noexcept { return pstl_domain<test_std::start_t>{}; }
};

struct pstl_env2 {
    template <typename Env>
    static auto query(test_std::get_completion_domain_t<>, const Env&) noexcept {
        return pstl_domain<test_std::set_value_t>{};
    }
};

struct pstl_just_sender {
    using sender_concept = test_std::sender_tag;

    template <typename Rcvr>
    struct state {
        using operation_state_concept = test_std::operation_state_tag;

        auto start() & noexcept -> void { test_std::set_value(std::move(rcvr)); }

        Rcvr rcvr;
    };

    template <typename...>
    static consteval auto get_completion_signatures() noexcept {
        return test_std::completion_signatures<test_std::set_value_t()>{};
    }

    template <typename Rcvr>
    auto connect(Rcvr rcvr) && noexcept {
        return state<Rcvr>{std::move(rcvr)};
    }

    static auto get_env() noexcept { return pstl_env2{}; }
};

static_assert(test_std::sender<pstl_just_sender>);

auto test_bulk_customization() {
    {
        // starting-domain customization
        std::vector<int> vec(32);
        std::ranges::iota(vec, 1);
        std::vector<int> result(vec.size());
        test_std::sync_wait(
            test_std::just() |
            test_std::bulk(test_std::par, vec.size(), [&](std::size_t i) noexcept { result[i] = vec[i] * vec[i]; }) |
            test_std::write_env(pstl_env1{}));

        for (std::size_t i = 0; i < vec.size(); ++i) {
            ASSERT(result[i] == vec[i] * vec[i]);
        }
    }
    {
        // completing-domain customization
        test::use_type<pstl_just_sender::sender_concept>();
        std::vector<int> vec(32);
        std::ranges::iota(vec, 1);
        std::vector<int> result(vec.size());
        test_std::sync_wait(
            pstl_just_sender{} |
            test_std::bulk(test_std::par, vec.size(), [&](std::size_t i) noexcept { result[i] = vec[i] * 2; }));

        for (std::size_t i = 0; i < vec.size(); ++i) {
            ASSERT(result[i] == vec[i] * 2);
        }
    }
}
#endif

} // namespace

TEST(exec_bulk) {

    try {

        test_bulk();
        test_bulk_noexcept();
        test_bulk_pipeable();
        test_bulk_chunked();
        test_bulk_chunked_with_values();
        test_bulk_chunked_pipeable();
        test_bulk_unchunked();
        test_bulk_unchunked_with_values();
        test_bulk_unchunked_pipeable();
        test_execution_policies();
        test_bulk_shape_zero();
        test_bulk_exception_handling();
        test_bulk_chunked_exception_handling();
        test_bulk_unchunked_exception_handling();
        test_bulk_chunked_noexcept();
        test_bulk_unchunked_noexcept();
        test_bulk_shape_one();
        test_bulk_chunked_covers_full_range();
        test_bulk_multiple_values();
#ifdef __cpp_lib_parallel_algorithm
        test_bulk_customization();
#endif
    } catch (...) {

        ASSERT(nullptr == +"the bulk tests shouldn't throw");
    }

    return EXIT_SUCCESS;
}
