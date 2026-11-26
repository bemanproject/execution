// src/beman/execution/tests/exec-then.test.cpp                     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <test/execution.hpp>
#include <test/attribute_sender.hpp>
#include <test/completion_test.hpp>
#include <test/sender_env.hpp>
#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <concepts>
#include <exception>
#include <memory_resource>
#include <optional>
#include <tuple>
#include <utility>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution/detail/then.hpp>
#include <beman/execution/detail/completion_signatures_of_t.hpp>
#include <beman/execution/detail/connect.hpp>
#include <beman/execution/detail/forwarding_query.hpp>
#include <beman/execution/detail/read_env.hpp>
#include <beman/execution/detail/get_stop_token.hpp>
#include <beman/execution/detail/inplace_stop_source.hpp>
#include <beman/execution/detail/into_variant.hpp>
#include <beman/execution/detail/run_loop.hpp>
#include <beman/execution/detail/schedule.hpp>
#include <beman/execution/detail/stopped_as_error.hpp>
#include <beman/execution/detail/stopped_as_optional.hpp>
#include <beman/execution/detail/just.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/sender_in.hpp>
#include <beman/execution/detail/start.hpp>
#include <beman/execution/detail/sync_wait.hpp>
#endif

// ----------------------------------------------------------------------------

namespace {
struct error {
    int value{};
};
struct non_sender {};

struct get_value_t : test_std::forwarding_query_t {
    auto operator()(const auto& e) const noexcept -> int { return e.query(*this); }
};

struct receiver {
    using receiver_concept = test_std::receiver_tag;

    auto set_error(auto&&) && noexcept -> void {}
    auto set_stopped() && noexcept -> void {}
    auto set_value(auto&&...) && noexcept -> void {}

    struct env {
        auto query(const get_value_t&) const noexcept { return 42; }
    };
    auto get_env() const noexcept { return env{}; }
};

template <typename... T>
struct sender {
    using sender_concept        = test_std::sender_tag;
    using completion_signatures = test_std::completion_signatures<T...>;
    template <typename, typename...>
    static consteval auto get_completion_signatures() -> completion_signatures {
        return {};
    }
};

constexpr auto consume_all = [](auto&&...) {};

template <bool Expect>
auto test_has(auto cpo, auto in_sender, auto fun) -> void {
    static_assert(test_std::receiver<receiver>);
    static_assert(Expect == requires {
        { cpo(in_sender, fun) } -> test_std::sender;
    });
    if constexpr (Expect) {
        static_assert(requires { cpo(fun); });
        static_assert(requires { in_sender | cpo(fun); });
        static_assert(requires {
            { in_sender | cpo(fun) } -> test_std::sender;
        });
        static_assert(requires {
            { in_sender | cpo(fun) | cpo(consume_all) } -> test_std::sender;
        });
        auto sender{cpo(in_sender, fun)};
        auto op{test_std::connect(::std::move(sender), receiver{})};
        test_std::start(op);

        const auto csender{cpo(in_sender, fun)};
        auto       cop{test_std::connect(std::move(csender), receiver{})};
        test_std::start(cop);
    }
}

template <typename... Completions>
auto test_then_type(auto sender) {
    static_assert(std::same_as<test_std::completion_signatures<Completions...>,
                               test_std::completion_signatures_of_t<decltype(sender), test_std::env<>>>);
}

auto test_then_type() -> void {
    test_then_type<test_std::set_value_t()>(test_std::just(0) | test_std::then([](auto) noexcept {}));
    test_then_type<test_std::set_value_t(int)>(test_std::just() | test_std::then([]() noexcept { return 0; }));
    test_then_type<test_std::set_error_t(int)>(test_std::just_error(0) | test_std::then([]() noexcept { return 0; }));
    test_then_type<test_std::set_stopped_t()>(test_std::just_stopped() | test_std::then([]() noexcept { return 0; }));

    test_then_type<test_std::set_value_t()>(test_std::just() | test_std::upon_error([]() noexcept { return 0; }));
    test_then_type<test_std::set_value_t(int)>(test_std::just_error(error{}) |
                                               test_std::upon_error([](error) noexcept { return 0; }));
    test_then_type<test_std::set_stopped_t()>(test_std::just_stopped() | test_std::upon_error([]() noexcept {}));

    test_then_type<test_std::set_value_t(int)>(test_std::just(0) | test_std::upon_stopped([]() noexcept {}));
    test_then_type<test_std::set_error_t(int)>(test_std::just_error(0) | test_std::upon_stopped([]() noexcept {}));
    test_then_type<test_std::set_value_t()>(test_std::just_stopped() | test_std::upon_stopped([]() noexcept {}));
}

auto test_then_multi_type() -> void {
    static_assert(test_std::sender<sender<>>);
    static_assert(test_std::sender_in<sender<>>);

    test_then_type<test_std::set_value_t(),
                   test_std::set_value_t(int, int),
                   test_std::set_error_t(error),
                   test_std::set_stopped_t()>(sender<test_std::set_value_t(),
                                                     test_std::set_value_t(int, int),
                                                     test_std::set_error_t(error),
                                                     test_std::set_stopped_t()>());
    test_then_type<test_std::set_value_t(bool),
                   test_std::set_error_t(error),
                   test_std::set_stopped_t(),
                   test_std::set_error_t(::std::exception_ptr)>(sender<test_std::set_value_t(),
                                                                       test_std::set_value_t(int, int),
                                                                       test_std::set_error_t(error),
                                                                       test_std::set_stopped_t()>() |
                                                                test_std::then([](auto&&...) { return true; }));
    test_then_type<test_std::set_value_t(),
                   test_std::set_value_t(int, int),
                   test_std::set_value_t(bool),
                   test_std::set_stopped_t(),
                   test_std::set_error_t(::std::exception_ptr)>(sender<test_std::set_value_t(),
                                                                       test_std::set_value_t(int, int),
                                                                       test_std::set_error_t(error),
                                                                       test_std::set_stopped_t()>() |
                                                                test_std::upon_error([](error) { return true; }));
    test_then_type<test_std::set_value_t(),
                   test_std::set_value_t(int, int),
                   test_std::set_stopped_t(),
                   test_std::set_error_t(::std::exception_ptr)>(sender<test_std::set_value_t(),
                                                                       test_std::set_value_t(int, int),
                                                                       test_std::set_error_t(error),
                                                                       test_std::set_stopped_t()>() |
                                                                test_std::upon_error([](error) {}));
    test_then_type<test_std::set_value_t(int),
                   test_std::set_value_t(int, int),
                   test_std::set_error_t(error),
                   test_std::set_value_t()>(sender<test_std::set_value_t(int),
                                                   test_std::set_value_t(int, int),
                                                   test_std::set_error_t(error),
                                                   test_std::set_stopped_t()>() |
                                            test_std::upon_stopped([]() noexcept {}));
    test_then_type<test_std::set_value_t(), test_std::set_value_t(int, int), test_std::set_error_t(error)>(
        sender<test_std::set_value_t(),
               test_std::set_value_t(int, int),
               test_std::set_error_t(error),
               test_std::set_stopped_t()>() |
        test_std::upon_stopped([]() noexcept {}));
    test_then_type<test_std::set_value_t(),
                   test_std::set_value_t(int, int),
                   test_std::set_error_t(error),
                   test_std::set_error_t(::std::exception_ptr)>(sender<test_std::set_value_t(),
                                                                       test_std::set_value_t(int, int),
                                                                       test_std::set_error_t(error),
                                                                       test_std::set_stopped_t()>() |
                                                                test_std::upon_stopped([] {}));
}

auto test_then_value() -> void {
    ASSERT(
        std::tuple{17} ==
        test_std::sync_wait(test_std::just(5, 12) | test_std::then([](int a, int b) { return a + b; })).value_or(0));
    ASSERT(std::tuple{17} ==
           test_std::sync_wait(test_std::just_error(17) | test_std::upon_error([](int a) { return a; })));
    ASSERT(std::tuple{17} ==
           test_std::sync_wait(test_std::just_stopped() | test_std::upon_stopped([]() { return 17; })));
}

struct memory_env {
    std::pmr::polymorphic_allocator<> allocator;
    auto                              query(const test_std::get_allocator_t&) const noexcept { return allocator; }
};
struct memory_receiver {
    using receiver_concept = test_std::receiver_tag;
    std::pmr::polymorphic_allocator<> allocator;
    auto                              get_env() const noexcept { return memory_env{this->allocator}; }

    auto set_error(auto&&) && noexcept -> void {}
    auto set_stopped() && noexcept -> void {}
    auto set_value(auto&&...) && noexcept -> void {}
};
struct counting_resource : std::pmr::memory_resource {
    std::size_t count{};
    auto        do_allocate(std::size_t size, std::size_t) -> void* override {
        ++this->count;
        return operator new(size);
    }
    auto do_deallocate(void* p, std::size_t, std::size_t) -> void override { operator delete(p); }
    auto do_is_equal(const std::pmr::memory_resource& other) const noexcept -> bool override { return this == &other; }
};

struct allocator_fun {
    using allocator_type = std::pmr::polymorphic_allocator<>;

    std::pmr::polymorphic_allocator<> alloc;
    std::byte*                        data{nullptr};

    explicit allocator_fun(std::pmr::polymorphic_allocator<> all) : alloc(all), data(alloc.allocate(1)) {}
    allocator_fun(const allocator_fun&, std::pmr::polymorphic_allocator<> = {}) {}
    allocator_fun(allocator_fun&& other) noexcept : alloc(other.alloc), data(std::exchange(other.data, nullptr)) {}
    allocator_fun(allocator_fun&& other, std::pmr::polymorphic_allocator<> all)
        : alloc(all), data(alloc == other.alloc ? std::exchange(other.data, nullptr) : alloc.allocate(1)) {}
    ~allocator_fun() {
        if (this->data)
            this->alloc.deallocate(this->data, 1u);
    }
    auto operator=(const allocator_fun&) -> allocator_fun = delete;
    auto operator=(allocator_fun&&) -> allocator_fun      = delete;

    auto operator()(auto&&...) const {}
};

auto test_then_allocator() -> void {
    static_assert(std::uses_allocator_v<allocator_fun, std::pmr::polymorphic_allocator<>>);
    counting_resource resource1{};
    ASSERT(resource1.count == 0u);
    auto sender{test_std::just() | test_std::then(allocator_fun(&resource1))};
    ASSERT(resource1.count == 1u);

    counting_resource resource2{};
    ASSERT(resource2.count == 0u);
    auto state{test_std::connect(std::move(sender), memory_receiver{&resource2})};
    ASSERT(resource2.count == 1u);
}

auto test_then_env() -> void {
    {
        receiver r{};
        auto     e{test_std::get_env(r)};
        ASSERT(e.query(get_value_t{}) == 42);
        ASSERT(get_value_t{}(e) == 42);
    }
    {
        auto state{test_std::connect(test_std::read_env(get_value_t{}), receiver{})};
        test_std::start(state);
    }
    {
        int  value{0};
        auto state{test_std::connect(test_std::then(test_std::read_env(get_value_t{}), [&value](int v) { value = v; }),
                                     receiver{})};
        ASSERT(value == 0);
        test_std::start(state);
        ASSERT(value == 42);
    }
}

auto test_then_completions() {
    test_std::sync_wait(test::completion_test(test_std::just() | test_std::then([]() {})));
    test_std::sync_wait(test::completion_test(test_std::just() | test_std::then([]() noexcept {})));
    test_std::sync_wait(test::completion_test(test_std::just() | test_std::then([](auto&&...) noexcept {})));
}

auto test_then_attributes() {
    test::sender_env s{42};
    test::test_sender_env<true>(42, test::test_forwardable_attr{}, s);
    test::test_sender_env<true>(84, test::test_non_forwardable_attr{}, s);
    test::test_sender_env<true>(42, test::test_forwardable_attr{}, s | test_std::then([]() {}));
    test::test_sender_env<false>(84, test::test_non_forwardable_attr{}, s | test_std::then([]() {}));
    test::test_sender_env<true>(42, test::test_forwardable_attr{}, s | test_std::upon_error([](auto) {}));
    test::test_sender_env<false>(84, test::test_non_forwardable_attr{}, s | test_std::upon_error([](auto) {}));
    test::test_sender_env<true>(42, test::test_forwardable_attr{}, s | test_std::upon_stopped([]() {}));
    test::test_sender_env<false>(84, test::test_non_forwardable_attr{}, s | test_std::upon_stopped([]() {}));
}

// Completion attributes must follow the source completion after an adaptor changes its tag.
struct attribute_domain : test_std::default_domain {};
using test::attribute_sender;
using test::error_domain;
using test::has_completion_scheduler;
using test::loop_scheduler;
using test::stopped_domain;
using test::value_domain;

auto test_remapped_completion_attributes() -> void {
    test_std::run_loop value_loop, error_loop, stopped_loop;
    struct receiver_env {
        loop_scheduler scheduler;
        auto           query(test_std::get_start_scheduler_t) const noexcept { return scheduler; }
        static auto    query(test_std::get_domain_t) noexcept { return attribute_domain{}; }
    };
    receiver_env env{value_loop.get_scheduler()};
    auto         recovered = test_std::just_error(-1) | test_std::upon_error([](int i) noexcept { return i; });
    auto         stopped   = test_std::just_stopped() | test_std::stopped_as_error(-1);
    ASSERT(test_std::get_completion_scheduler<test_std::set_value_t>(test_std::get_env(recovered), env) ==
           value_loop.get_scheduler());
    ASSERT(test_std::get_completion_scheduler<test_std::set_error_t>(test_std::get_env(stopped), env) ==
           value_loop.get_scheduler());
    static_assert(std::same_as<decltype(test_std::get_completion_domain<test_std::set_value_t>(
                                   test_std::get_env(recovered), env)),
                               attribute_domain>);
    static_assert(
        std::same_as<decltype(test_std::get_completion_domain<test_std::set_error_t>(test_std::get_env(stopped), env)),
                     attribute_domain>);

    auto child =
        attribute_sender<>{value_loop.get_scheduler(), error_loop.get_scheduler(), stopped_loop.get_scheduler()};
    auto mixed_value = child | test_std::upon_error([](int i) noexcept { return i; });
    auto mixed_error = child | test_std::stopped_as_error(-1);
    auto unchanged   = child | test_std::then([](int i) noexcept { return i; });
    auto throwing    = child | test_std::then([](int i) { return i; });
    auto optional    = child | test_std::stopped_as_optional;
    auto empty_env   = test_std::env{};
    static_assert(!has_completion_scheduler<test_std::set_value_t, decltype(mixed_value), decltype(empty_env)>);
    static_assert(!has_completion_scheduler<test_std::set_error_t, decltype(mixed_error), decltype(empty_env)>);
    static_assert(std::same_as<decltype(test_std::get_completion_domain<test_std::set_value_t>(
                                   test_std::get_env(mixed_value), empty_env)),
                               test_std::indeterminate_domain<value_domain, error_domain>>);
    static_assert(std::same_as<decltype(test_std::get_completion_domain<test_std::set_error_t>(
                                   test_std::get_env(mixed_error), empty_env)),
                               test_std::indeterminate_domain<error_domain, stopped_domain>>);
    static_assert(!has_completion_scheduler<test_std::set_error_t, decltype(throwing), decltype(empty_env)>);
    static_assert(std::same_as<decltype(test_std::get_completion_domain<test_std::set_error_t>(
                                   test_std::get_env(throwing), empty_env)),
                               test_std::indeterminate_domain<value_domain, error_domain>>);
    static_assert(!has_completion_scheduler<test_std::set_value_t, decltype(optional), decltype(empty_env)>);
    static_assert(std::same_as<decltype(test_std::get_completion_domain<test_std::set_value_t>(
                                   test_std::get_env(optional), empty_env)),
                               test_std::indeterminate_domain<value_domain, stopped_domain>>);
    ASSERT(test_std::get_completion_scheduler<test_std::set_error_t>(test_std::get_env(unchanged), empty_env) ==
           error_loop.get_scheduler());
    auto same_origin = test_std::just(1) | test_std::then([](int i) { return i; }) |
                       test_std::upon_error([](const std::exception_ptr&) noexcept { return -1; });
    ASSERT(test_std::get_completion_scheduler<test_std::set_value_t>(test_std::get_env(same_origin), env) ==
           value_loop.get_scheduler());
}

auto test_equal_schedulers_from_independent_sources() -> void {
    test_std::run_loop loop;
    const auto         scheduler = loop.get_scheduler();
    auto               child     = attribute_sender<>{scheduler, scheduler, scheduler};
    auto               attrs     = test_std::get_env(child);
    ASSERT(test_std::get_completion_scheduler<test_std::set_value_t>(attrs) ==
           test_std::get_completion_scheduler<test_std::set_error_t>(attrs));

    auto sndr = child | test_std::upon_error([](int i) noexcept { return i; });
    // Equal scheduler values do not make two independent completion queries the same source.
    static_assert(!has_completion_scheduler<test_std::set_value_t, decltype(sndr)>);
    static_assert(!has_completion_scheduler<test_std::set_value_t, decltype(sndr), test_std::env<>>);
    ASSERT(test_std::get_completion_scheduler<test_std::set_stopped_t>(test_std::get_env(sndr)) == scheduler);
}

struct potentially_throwing_copy {
    potentially_throwing_copy(const potentially_throwing_copy&) noexcept(false);
    potentially_throwing_copy(potentially_throwing_copy&&) noexcept = default;
};

auto test_optional_construction_error_origin() -> void {
    test_std::run_loop value_loop, error_loop;
    const auto         value_scheduler = value_loop.get_scheduler();
    const auto         error_scheduler = error_loop.get_scheduler();
    const auto         env             = test_std::env<>{};
    auto               copy_child =
        attribute_sender<const potentially_throwing_copy&>{value_scheduler, error_scheduler, value_scheduler};
    auto move_child = attribute_sender<potentially_throwing_copy>{value_scheduler, error_scheduler, value_scheduler};
    auto copy       = copy_child | test_std::stopped_as_optional;
    auto move       = move_child | test_std::stopped_as_optional;

    static_assert(!has_completion_scheduler<test_std::set_error_t, decltype(copy), decltype(env)>);
    static_assert(
        std::same_as<decltype(test_std::get_completion_domain<test_std::set_error_t>(test_std::get_env(copy), env)),
                     test_std::indeterminate_domain<value_domain, error_domain>>);
    ASSERT(test_std::get_completion_scheduler<test_std::set_error_t>(test_std::get_env(move), env) == error_scheduler);
}

auto test_variant_construction_error_origin() -> void {
    test_std::run_loop value_loop, error_loop;
    const auto         value_scheduler = value_loop.get_scheduler();
    const auto         error_scheduler = error_loop.get_scheduler();
    const auto         env             = test_std::env<>{};
    auto               copy_child =
        attribute_sender<const potentially_throwing_copy&>{value_scheduler, error_scheduler, value_scheduler};
    auto move_child = attribute_sender<potentially_throwing_copy>{value_scheduler, error_scheduler, value_scheduler};
    auto copy       = copy_child | test_std::into_variant;
    auto move       = move_child | test_std::into_variant;

    using copy_signatures = test_std::completion_signatures_of_t<decltype(copy), test_std::env<>>;
    using move_signatures = test_std::completion_signatures_of_t<decltype(move), test_std::env<>>;
    static_assert(copy_signatures::count_of(test_std::set_error_t{}) == 2);
    static_assert(move_signatures::count_of(test_std::set_error_t{}) == 1);
    static_assert(!has_completion_scheduler<test_std::set_error_t, decltype(copy), decltype(env)>);
    static_assert(
        std::same_as<decltype(test_std::get_completion_domain<test_std::set_error_t>(test_std::get_env(copy), env)),
                     test_std::indeterminate_domain<value_domain, error_domain>>);
    ASSERT(test_std::get_completion_scheduler<test_std::set_error_t>(test_std::get_env(move), env) == error_scheduler);
    ASSERT(test_std::get_completion_scheduler<test_std::set_value_t>(test_std::get_env(copy), env) == value_scheduler);
}

auto test_scheduler_origin_through_nested_adaptors() -> void {
    test_std::run_loop            loop;
    test_std::inplace_stop_source stop_source;
    struct stoppable_env {
        test_std::inplace_stop_token token;
        auto                         query(test_std::get_stop_token_t) const noexcept { return token; }
    };
    const auto env       = stoppable_env{stop_source.get_token()};
    const auto scheduler = loop.get_scheduler();
    auto       sndr      = test_std::schedule(scheduler) | test_std::then([] { return 42; }) |
                           test_std::upon_error([](const std::exception_ptr&) { return -1; }) |
                           test_std::upon_error([](const std::exception_ptr&) noexcept { return -2; });
    auto       attrs     = test_std::get_env(sndr);
    // Each recovery can turn a callback failure back into a value. All value paths still
    // trace to schedule's value completion, independently of the possible stopped path.
    ASSERT(test_std::get_completion_scheduler<test_std::set_value_t>(attrs, env) == scheduler);
    ASSERT(test_std::get_completion_scheduler<test_std::set_stopped_t>(attrs, env) == scheduler);
    static_assert(!has_completion_scheduler<test_std::set_error_t, decltype(sndr), decltype(env)>);
}
} // namespace

TEST(exec_then) {
    static_assert(std::same_as<const test_std::then_t, decltype(test_std::then)>);
    static_assert(std::same_as<const test_std::upon_error_t, decltype(test_std::upon_error)>);
    static_assert(std::same_as<const test_std::upon_stopped_t, decltype(test_std::upon_stopped)>);

    test_has<false>(test_std::then, non_sender{}, [] {});
    test_has<true>(test_std::then, test_std::just(), [] {});
    test_has<true>(test_std::then, test_std::just_error(error{3}), [] {});
    test_has<true>(test_std::then, test_std::just_stopped(), [] {});

    test_then_type();
    test_then_multi_type();
    test_then_value();
    test_then_allocator();

    test_then_env();
    test_then_completions();
    test_then_attributes();
    test_remapped_completion_attributes();
    test_equal_schedulers_from_independent_sources();
    test_optional_construction_error_origin();
    test_variant_construction_error_origin();
    test_scheduler_origin_through_nested_adaptors();
}
