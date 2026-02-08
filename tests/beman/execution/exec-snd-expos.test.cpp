// src/beman/execution/tests/exec-snd-expos.test.cpp                 -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <concepts>
#include <utility>
#include <test/execution.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
import beman.execution.detail;
#else
#include <beman/execution/detail/child_type.hpp>
#include <beman/execution/detail/write_env.hpp>
#include <beman/execution/detail/make_sender.hpp>
#include <beman/execution/detail/basic_sender.hpp>
#include <beman/execution/detail/completion_signatures_for.hpp>
#include <beman/execution/detail/connect_all_result.hpp>
#include <beman/execution/detail/product_type.hpp>
#include <beman/execution/detail/operation_state.hpp>
#include <beman/execution/detail/basic_operation.hpp>
#include <beman/execution/detail/connect_all.hpp>
#include <beman/execution/detail/fwd_env.hpp>
#include <beman/execution/detail/make_env.hpp>
#include <beman/execution/detail/join_env.hpp>
#include <beman/execution/detail/sched_attrs.hpp>
#include <beman/execution/detail/sched_env.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/query_with_default.hpp>
#include <beman/execution/detail/get_domain_early.hpp>
#include <beman/execution/detail/get_domain_late.hpp>
#include <beman/execution/detail/default_impls.hpp>
#include <beman/execution/detail/impls_for.hpp>
#include <beman/execution/detail/state_type.hpp>
#include <beman/execution/detail/basic_state.hpp>
#include <beman/execution/detail/indices_for.hpp>
#include <beman/execution/detail/valid_specialization.hpp>
#include <beman/execution/detail/env_type.hpp>
#include <beman/execution/detail/basic_receiver.hpp>
#include <beman/execution/detail/completion_tag.hpp>
#include <beman/execution/detail/scheduler.hpp>
#include <beman/execution/execution.hpp>
#include <beman/execution/detail/tag_of_t.hpp>

#include <beman/execution/detail/suppress_push.hpp>
#endif

// ----------------------------------------------------------------------------

namespace {
struct domain {
    int  value{};
    auto operator==(const domain&) const -> bool = default;
};
struct default_domain {
    int default_value{};
};

constexpr struct forwardable_t : test_std::forwarding_query_t {
} forwardable;
struct non_forwardable_t {};

template <int>
struct custom_query_t {};
template <int I>
constexpr custom_query_t<I> custom_query;
struct custom_result {
    int  value;
    auto operator==(const custom_result&) const -> bool = default;
};

struct env {
    int  value{};
    auto query(const test_std::get_domain_t&) const noexcept { return domain{value}; }
    auto query(const non_forwardable_t&) const noexcept { return true; }
    auto query(const forwardable_t&, int a, int b) const noexcept { return (value + a) * b; }
};

struct env1 {
    int  value{};
    auto query(custom_query_t<0>, int a) const noexcept { return this->value + a; }
    auto query(custom_query_t<1>, int a) const noexcept { return this->value + a; }
};

struct env2 {
    int  value{};
    auto query(custom_query_t<0>, int a) const noexcept { return this->value + a; }
    auto query(custom_query_t<2>, int a) const noexcept { return this->value + a; }
};

struct test_sender {
    using sender_concept = test_std::sender_t;
};

struct scheduler {
    int  value{};
    auto query(test_std::get_domain_t) const { return domain{this->value}; }

    auto operator==(const scheduler&) const -> bool = default;
};

struct tag {
    static auto name() { return "expos test anonymous"; }
};

template <typename Receiver>
struct operation_state : test_detail::immovable {
    using operation_state_concept = test_std::operation_state_t;
    int* counter;
    explicit operation_state(int* cntr) : counter(cntr) {}
    auto start() & noexcept -> void { ++*counter; }
};

struct sender0 {
    struct env {};
    using sender_concept = test_std::sender_t;
    using indices_for    = ::std::index_sequence_for<>;
    tag  t{};
    int* data{};
    template <typename Receiver>
    auto connect(Receiver&&) const noexcept {
        return operation_state<Receiver>(data);
    }
    auto get_env() const noexcept -> env { return {}; }
};

struct sender1 {
    using sender_concept = test_std::sender_t;
    using indices_for    = ::std::index_sequence_for<sender0>;
    tag     t{};
    int*    data{};
    sender0 c0{};
    template <typename Receiver>
    auto connect(Receiver&&) const noexcept {
        return operation_state<Receiver>(data);
    }
};

struct sender2 {
    using sender_concept = test_std::sender_t;
    using indices_for    = ::std::index_sequence_for<sender0, sender0>;
    tag     t{};
    int*    data{};
    sender0 c0{};
    sender0 c1{};
    template <typename Receiver>
    auto connect(Receiver&&) const noexcept {
        return operation_state<Receiver>(data);
    }
};

struct sender3 {
    using sender_concept = test_std::sender_t;
    using indices_for    = ::std::index_sequence_for<sender0, sender0, sender0>;
    tag     t{};
    int*    data{};
    sender0 c0{};
    sender0 c1{};
    sender0 c2{};
    template <typename Receiver>
    auto connect(Receiver&&) const noexcept {
        return operation_state<Receiver>(data);
    }
};

struct sender4 {
    using sender_concept = test_std::sender_t;
    using indices_for    = ::std::index_sequence_for<sender0, sender0, sender0, sender0>;
    tag     t{};
    int*    data{};
    sender0 c0{};
    sender0 c1{};
    sender0 c2{};
    sender0 c3{};
    template <typename Receiver>
    auto connect(Receiver&&) const noexcept {
        return operation_state<Receiver>(data);
    }
};

struct receiver {
    using receiver_concept = test_std::receiver_t;
    auto set_value(auto&&...) noexcept -> void {}
    auto set_error(auto&&) noexcept -> void {}
    auto set_stopped() noexcept -> void {}
};

// ------------------------------------------------------------------------

template <bool Expect, typename Query>
auto test_fwd_env_helper() -> void {
    env e{42};
    static_assert(Expect == requires() { test_detail::fwd_env(e).query(Query()); });
}
auto test_fwd_env() -> void {
    env e{42};
    static_assert(test_detail::queryable<decltype(test_detail::fwd_env(e))>);
    test_fwd_env_helper<true, test_std::get_domain_t>();
    test_fwd_env_helper<false, non_forwardable_t>();
    ASSERT(129 == test_detail::fwd_env(e).query(forwardable, 1, 3));
}

auto test_make_env() -> void {
    auto       env{test_detail::make_env(custom_query<0>, custom_result{43})};
    const auto cenv{env};
    static_assert(test_detail::queryable<decltype(env)>);
    ASSERT(env.query(custom_query<0>) == custom_result{43});
    ASSERT(cenv.query(custom_query<0>) == custom_result{43});
}

template <bool Expect, typename Query, typename Env>
auto test_join_env(Env&& env) -> void {
    static_assert(Expect == requires { env.query(Query(), 17); });
}
auto test_join_env() -> void {
    env1       e1{42};
    const env1 ce1{e1};
    env2       e2{17};
    const env2 ce2{e2};

    test_join_env<true, custom_query_t<0>>(e1);
    test_join_env<true, custom_query_t<0>>(ce1);
    test_join_env<true, custom_query_t<1>>(e1);
    test_join_env<true, custom_query_t<1>>(ce1);
    test_join_env<false, custom_query_t<2>>(e1);
    test_join_env<false, custom_query_t<2>>(ce1);
    test_join_env<false, custom_query_t<3>>(e1);
    test_join_env<false, custom_query_t<3>>(ce1);
    test_join_env<true, custom_query_t<0>>(e2);
    test_join_env<true, custom_query_t<0>>(ce2);
    test_join_env<false, custom_query_t<1>>(e2);
    test_join_env<false, custom_query_t<1>>(ce2);
    test_join_env<true, custom_query_t<2>>(e2);
    test_join_env<true, custom_query_t<2>>(ce2);
    test_join_env<false, custom_query_t<3>>(e2);
    test_join_env<false, custom_query_t<3>>(ce2);

    auto        env{test_detail::join_env(e1, e2)};
    const auto& cenv{env};

    static_assert(test_detail::queryable<decltype(env)>);
    test_join_env<true, custom_query_t<0>>(env);
    test_join_env<true, custom_query_t<0>>(cenv);
    test_join_env<true, custom_query_t<1>>(env);
    test_join_env<true, custom_query_t<1>>(cenv);
    test_join_env<true, custom_query_t<2>>(env);
    test_join_env<true, custom_query_t<2>>(cenv);
    test_join_env<false, custom_query_t<3>>(env);
    test_join_env<false, custom_query_t<3>>(cenv);

    ASSERT(env.query(custom_query<0>, 13) == 55);
    ASSERT(cenv.query(custom_query<0>, 13) == 55);
    ASSERT(env.query(custom_query<1>, 13) == 55);
    ASSERT(cenv.query(custom_query<1>, 13) == 55);
    ASSERT(env.query(custom_query<2>, 13) == 30);
    ASSERT(cenv.query(custom_query<2>, 13) == 30);
}

auto test_sched_attrs() -> void {
    scheduler sched{17};
    ASSERT(sched.query(test_std::get_domain) == domain{17});

    auto attrs{test_detail::sched_attrs(sched)};
    static_assert(test_detail::queryable<decltype(attrs)>);

    auto sched_error{attrs.query(test_std::get_completion_scheduler<test_std::set_error_t>)};
    static_assert(::std::same_as<scheduler, decltype(sched_error)>);
    ASSERT(sched_error == sched);

    auto sched_stopped{attrs.query(test_std::get_completion_scheduler<test_std::set_stopped_t>)};
    static_assert(::std::same_as<scheduler, decltype(sched_stopped)>);
    ASSERT(sched_stopped == sched);

    auto sched_value{attrs.query(test_std::get_completion_scheduler<test_std::set_value_t>)};
    static_assert(::std::same_as<scheduler, decltype(sched_value)>);
    ASSERT(sched_value == sched);

    auto dom{attrs.query(test_std::get_domain)};
    static_assert(::std::same_as<domain, decltype(dom)>);
    ASSERT(dom == domain{17});
}

auto test_sched_env() -> void {
    scheduler sched{17};
    ASSERT(sched.query(test_std::get_domain) == domain{17});

    auto env{test_detail::sched_env(sched)};
    static_assert(test_detail::queryable<decltype(env)>);

    auto qsched{env.query(test_std::get_scheduler)};
    static_assert(::std::same_as<scheduler, decltype(qsched)>);
    ASSERT(qsched == sched);

    auto dom{env.query(test_std::get_domain)};
    static_assert(::std::same_as<domain, decltype(dom)>);
    ASSERT(dom == domain{17});
}

namespace completion_domain {
struct domain {};
struct other {};
struct common {};
struct none {};
struct mismatch {};

template <typename>
struct env;
template <typename>
struct scheduler;

template <typename W>
struct sender {
    using sender_concept = test_std::sender_t;
    auto get_env() const noexcept -> env<W> { return {}; }
};
template <typename W>
struct scheduler {
    using scheduler_concept = test_std::scheduler_t;
    auto schedule() noexcept -> sender<W> { return {}; }
    auto operator==(const scheduler&) const -> bool = default;
    auto query(const test_std::get_domain_t&) const noexcept -> domain { return {}; }
};

template <>
struct scheduler<none> {
    using scheduler_concept = test_std::scheduler_t;
    auto schedule() noexcept -> sender<none> { return {}; }
    auto operator==(const scheduler&) const -> bool = default;
};

template <>
struct env<domain> {
    template <typename Tag>
    auto query(const test_std::get_completion_scheduler_t<Tag>&) const noexcept -> scheduler<domain> {
        return {};
    }
};

template <>
struct env<common> {
    template <typename Tag>
    auto query(const test_std::get_completion_scheduler_t<Tag>&) const noexcept -> scheduler<common> {
        return {};
    }
};

template <>
struct env<none> {
    template <typename Tag>
    auto query(const test_std::get_completion_scheduler_t<Tag>&) const noexcept -> scheduler<none> {
        return {};
    }
};

struct one_missing {};
template <>
struct env<one_missing> {
    template <typename Tag>
    auto query(const test_std::get_completion_scheduler_t<Tag>&) const noexcept -> scheduler<one_missing> {
        return {};
    }
    auto query(const test_std::get_completion_scheduler_t<test_std::set_error_t>&) const noexcept = delete;
};

struct two_missing {};
template <>
struct env<two_missing> {
    auto query(const test_std::get_completion_scheduler_t<test_std::set_value_t>&) const noexcept
        -> scheduler<two_missing> {
        return {};
    }
};
} // namespace completion_domain
auto test_completion_domain() -> void {
    namespace cd = completion_domain;
    static_assert(test_std::sender<cd::sender<cd::common>>);
    static_assert(std::same_as<cd::env<cd::common>, decltype(test_std::get_env(cd::sender<cd::common>{}))>);
    static_assert(std::same_as<cd::domain,
                               decltype(test_std::get_domain(test_std::get_completion_scheduler<test_std::set_error_t>(
                                   cd::env<cd::common>{})))>);
    static_assert(
        std::same_as<cd::domain,
                     decltype(test_std::get_domain(
                         test_std::get_completion_scheduler<test_std::set_stopped_t>(cd::env<cd::common>{})))>);
    static_assert(std::same_as<cd::domain,
                               decltype(test_std::get_domain(test_std::get_completion_scheduler<test_std::set_value_t>(
                                   cd::env<cd::common>{})))>);
    static_assert(std::same_as<cd::domain, decltype(test_detail::completion_domain((cd::sender<cd::common>{})))>);
    static_assert(std::same_as<cd::domain, decltype(test_detail::completion_domain((cd::sender<cd::one_missing>{})))>);
    static_assert(std::same_as<cd::domain, decltype(test_detail::completion_domain((cd::sender<cd::two_missing>{})))>);
    static_assert(
        std::same_as<test_std::default_domain, decltype(test_detail::completion_domain((cd::sender<cd::none>{})))>);
}

auto test_query_with_default() -> void {
    auto result1{test_detail::query_with_default(test_std::get_domain, env{43}, default_domain{74})};
    static_assert(std::same_as<domain, decltype(result1)>);
    ASSERT(result1.value == 43);

    auto result2{test_detail::query_with_default(test_std::get_domain, test_std::env<>(), default_domain{74})};
    static_assert(std::same_as<default_domain, decltype(result2)>);
    ASSERT(result2.default_value == 74);
}

auto test_get_domain_early() -> void {
    struct plain_sender {
        using sender_concept = test_std::sender_t;
    };
    static_assert(test_std::sender<plain_sender>);
    static_assert(std::same_as<test_std::default_domain, decltype(test_detail::get_domain_early(plain_sender{}))>);

    namespace cd = completion_domain;
    static_assert(test_std::sender<cd::sender<cd::domain>>);
    static_assert(std::same_as<cd::domain, decltype(test_detail::get_domain_early(cd::sender<cd::domain>{}))>);

    struct sender_with_domain {
        using sender_concept = test_std::sender_t;
        struct domain {};
        struct env {
            auto query(const test_std::get_domain_t&) const noexcept -> domain { return {}; }
        };
        auto get_env() const noexcept -> env { return {}; }
    };
    static_assert(test_std::sender<sender_with_domain>);
    static_assert(std::same_as<sender_with_domain::env, decltype(test_std::get_env(sender_with_domain{}))>);
    static_assert(std::same_as<sender_with_domain::domain, decltype(test_std::get_domain(sender_with_domain::env{}))>);
    static_assert(
        std::same_as<sender_with_domain::domain, decltype(test_detail::get_domain_early(sender_with_domain{}))>);
}

template <typename Expect>
auto test_get_domain_late(auto sender, auto env) -> void {
    static_assert(test_std::sender<decltype(sender)>);
    static_assert(test_detail::queryable<decltype(env)>);
    static_assert(std::same_as<Expect, decltype(test_detail::get_domain_late(sender, env))>);
}

struct get_domain_late_scheduler {
    struct dom {};
    struct env {
        template <typename Tag>
        auto query(const test_std::get_completion_scheduler_t<Tag>&) const noexcept -> get_domain_late_scheduler {
            return {};
        }
    };
    struct sender {
        using sender_concept = test_std::sender_t;
        auto get_env() const noexcept -> env { return {}; }
    };
    using scheduler_concept = test_std::scheduler_t;
    auto schedule() noexcept -> sender { return {}; }
    auto operator==(const get_domain_late_scheduler&) const -> bool = default;
    auto query(const test_std::get_domain_t&) const noexcept -> dom { return {}; }
};

struct get_domain_late_env {
    struct dom {};
    template <typename Tag>
    auto query(const test_std::get_completion_scheduler_t<Tag>&) const noexcept -> get_domain_late_scheduler {
        return {};
    }
    auto query(const test_std::get_domain_t&) const noexcept -> dom { return {}; }
};

auto test_get_domain_late() -> void {
    struct no_domain_sender {
        using sender_concept = test_std::sender_t;
    };
    static_assert(test_std::sender<no_domain_sender>);
    test_get_domain_late<test_std::default_domain>(no_domain_sender{}, test_std::env<>{});

    struct scheduler_env {
        auto query(const test_std::get_scheduler_t&) const noexcept -> get_domain_late_scheduler { return {}; }
    };
    static_assert(test_std::scheduler<get_domain_late_scheduler>);
    static_assert(std::same_as<get_domain_late_scheduler, decltype(test_std::get_scheduler(scheduler_env{}))>);
    static_assert(
        std::same_as<get_domain_late_scheduler::dom, decltype(test_std::get_domain(get_domain_late_scheduler{}))>);
    test_get_domain_late<get_domain_late_scheduler::dom>(no_domain_sender{}, scheduler_env{});

    struct domain_env {
        struct dom {};
        auto query(const test_std::get_scheduler_t&) const noexcept -> get_domain_late_scheduler { return {}; }
        auto query(test_std::get_domain_t) const noexcept -> dom { return {}; }
    };
    static_assert(std::same_as<domain_env::dom, decltype(test_std::get_domain(domain_env{}))>);
    test_get_domain_late<domain_env::dom>(no_domain_sender{}, domain_env{});

    struct scheduler_sender {
        using sender_concept = test_std::sender_t;
        auto get_env() const noexcept -> get_domain_late_scheduler::env { return {}; }
    };
    static_assert(test_std::sender<scheduler_sender>);
    static_assert(std::same_as<get_domain_late_scheduler::env, decltype(test_std::get_env(scheduler_sender{}))>);
    static_assert(
        std::same_as<get_domain_late_scheduler::dom, decltype(test_detail::completion_domain(scheduler_sender{}))>);
    test_get_domain_late<get_domain_late_scheduler::dom>(scheduler_sender{}, test_std::env<>{});

    struct env_sender {
        using sender_concept = test_std::sender_t;
        auto get_env() const noexcept -> get_domain_late_env { return {}; }
    };
    static_assert(test_std::sender<env_sender>);
    static_assert(std::same_as<get_domain_late_env::dom, decltype(test_std::get_domain(get_domain_late_env{}))>);
    static_assert(std::same_as<get_domain_late_env, decltype(test_std::get_env(env_sender{}))>);
    test_get_domain_late<get_domain_late_env::dom>(env_sender{}, test_std::env<>{});
}

auto test_default_impls_get_attrs() -> void {
    struct local_env {
        int value;
    };
    struct child1 {
        auto get_env() const noexcept { return local_env{1}; }
    };
    struct child2 {
        auto get_env() const noexcept { return local_env{2}; }
    };

    static_assert(noexcept(test_detail::default_impls::get_attrs(0, child1{})));
    static_assert(
        std::same_as<test_detail::fwd_env<local_env>, decltype(test_detail::default_impls::get_attrs(0, child1{}))>);
    //-dk:TODO static_assert(std::same_as<test_std::env<>,
    //     decltype(test_detail::default_impls::get_attrs(0, child1{}, child2{}))>);
}

auto test_default_impls_get_env() -> void {
    struct local_env {
        int value;
    };
    struct local_receiver {
        auto get_env() const noexcept { return local_env{1}; }
    };

    int arg{};
    static_assert(noexcept(test_detail::default_impls::get_env(0, arg, local_receiver{})));
    static_assert(std::same_as<test_detail::fwd_env<local_env>,
                               decltype(test_detail::default_impls::get_env(0, arg, local_receiver{}))>);
}

auto test_default_impls_get_state() -> void {
    struct local_tag {
        static auto name() { return "test_default_impls_get_state"; }
    };
    struct data {
        int  v1{};
        int  v2{};
        auto operator==(const data&) const -> bool = default;
    };
    struct local_sender0 {
        local_tag t{};
        data      d{1, 2};
    };
    struct local_sender1 {
        local_tag t{};
        data      d{1, 2};
        int       i1{};
    };
    struct local_sender2 {
        local_tag t{};
        data      d{1, 2};
        int       i1{};
        int       i2{};
    };
    struct local_sender3 {
        local_tag t{};
        data      d{1, 2};
        int       i1{};
        int       i2{};
        int       i3{};
    };
    struct local_sender4 {
        local_tag t{};
        data      d{1, 2};
        int       i1{};
        int       i2{};
        int       i3{};
        int       i4{};
    };
    struct local_receiver {};

    local_sender0       s{};
    const local_sender0 cs{};
    local_receiver      r{};
    static_assert(noexcept(test_detail::default_impls::get_state(local_sender0{}, r)));
    static_assert(std::same_as<data&&, decltype(test_detail::default_impls::get_state(local_sender0{}, r))>);
    ASSERT((data{1, 2}) == test_detail::default_impls::get_state(local_sender0{}, r));
    static_assert(std::same_as<data&&, decltype(test_detail::default_impls::get_state(local_sender1{}, r))>);
    ASSERT((data{1, 2}) == test_detail::default_impls::get_state(local_sender1{}, r));
    static_assert(std::same_as<data&&, decltype(test_detail::default_impls::get_state(local_sender2{}, r))>);
    ASSERT((data{1, 2}) == test_detail::default_impls::get_state(local_sender2{}, r));
    static_assert(std::same_as<data&&, decltype(test_detail::default_impls::get_state(local_sender3{}, r))>);
    ASSERT((data{1, 2}) == test_detail::default_impls::get_state(local_sender3{}, r));
    static_assert(std::same_as<data&&, decltype(test_detail::default_impls::get_state(local_sender4{}, r))>);
    ASSERT((data{1, 2}) == test_detail::default_impls::get_state(local_sender4{}, r));
    static_assert(std::same_as<data&, decltype(test_detail::default_impls::get_state(s, r))>);
    static_assert(std::same_as<const data&, decltype(test_detail::default_impls::get_state(cs, r))>);
}
auto test_default_impls_start() -> void {
    struct state {
        bool called{false};
        auto start() noexcept -> void { called = true; }
    };
    int   a0{};
    int   a1{};
    state s1;
    state s2;
    state s3;
    state s4;
    state s5;
    static_assert(noexcept(test_detail::default_impls::start(a0, a1, s1, s2, s3, s4, s5)));
    ASSERT(s1.called == false);
    ASSERT(s2.called == false);
    ASSERT(s3.called == false);
    ASSERT(s4.called == false);
    ASSERT(s5.called == false);
    test_detail::default_impls::start(a0, a1, s1, s2, s3, s4, s5);
    ASSERT(s1.called == true);
    ASSERT(s2.called == true);
    ASSERT(s3.called == true);
    ASSERT(s4.called == true);
    ASSERT(s5.called == true);
}
template <typename Impls>
auto test_default_impls_complete(Impls) -> void {
    struct arg {};
    struct local_receiver {
        bool& called;
    };
    struct state {};

    bool           called{false};
    auto           non_tag = [](local_receiver&&, int) {};
    auto           tag     = [](local_receiver&& r, int, arg) { r.called = true; };
    local_receiver r{called};
    state          s{};

    static_assert(not requires { Impls::complete(::std::integral_constant<int, 0>{}, s, r, non_tag, 0, arg{}); });
    static_assert(requires { Impls::complete(::std::integral_constant<int, 0>{}, s, r, tag, 0, arg{}); });
    static_assert(noexcept(Impls::complete(::std::integral_constant<int, 0>{}, s, r, tag, 0, arg{})));

    ASSERT(r.called == false);
    Impls::complete(::std::integral_constant<int, 0>{}, s, r, tag, 0, arg{});
    ASSERT(r.called == true);
}

auto test_default_impls() -> void {
    test_default_impls_get_attrs();
    test_default_impls_get_env();
    test_default_impls_get_state();
    test_default_impls_start();
    test_default_impls_complete(test_detail::default_impls{});
}

auto test_impls_for() -> void {
    struct local_tag {
        static auto name() { return "test_impls_for"; }
    };

    static_assert(std::derived_from<test_detail::impls_for<local_tag>, test_detail::default_impls>);
}

auto test_state_type() -> void {
    struct local_tag {
        static auto name() { return "test_state_type"; }
    };
    struct state {};
    struct sender {
        local_tag t;
        state     s;
    };
    struct local_receiver {};

    static_assert(std::same_as<state, test_detail::state_type<sender, local_receiver>>);
}

auto test_basic_state() -> void {
    struct local_tag {
        static auto name() { return "test_basic_state"; }
    };
    struct data {};
    struct local_sender {
        local_tag t;
        data      d;
    };
    struct local_receiver {};

    test_detail::basic_state<local_sender, local_receiver> state(local_sender{}, local_receiver{});
}

auto test_indices_for() -> void {
    using indices = ::std::make_index_sequence<5>;
    struct sender {
        using indices_for = indices;
    };

    static_assert(::std::same_as<indices, test_detail::indices_for<sender>>);
    static_assert(::std::same_as<indices, test_detail::indices_for<sender&>>);
    static_assert(::std::same_as<indices, test_detail::indices_for<sender&&>>);
    static_assert(::std::same_as<indices, test_detail::indices_for<const sender&>>);
    static_assert(::std::same_as<indices, test_detail::indices_for<const sender&&>>);
}

template <typename, typename>
struct test_valid_specializaton_template {};

auto test_valid_specialization() -> void {
    static_assert(test_detail::valid_specialization<test_valid_specializaton_template, int, int>);
    static_assert(not test_detail::valid_specialization<test_valid_specializaton_template, int>);
    static_assert(not test_detail::valid_specialization<test_valid_specializaton_template, int, int, int>);
}

auto test_env_type() -> void {
    using index = std::integral_constant<int, 0>;
    struct local_tag {
        static auto name() { return "test_env_type"; }
    };
    struct data {};
    struct local_env {};
    struct local_sender {
        local_tag t;
        data      d;
    };
    struct sender_with_env {
        local_tag t;
        data      d;
        auto      get_env() const noexcept -> local_env { return {}; }
    };
    struct local_receiver {};
    struct receiver_with_env {
        auto get_env() const noexcept -> local_env { return {}; }
    };

    static_assert(std::same_as<test_detail::fwd_env<test_std::env<>>,
                               test_detail::env_type<index, local_sender, local_receiver>>);
    static_assert(std::same_as<test_detail::fwd_env<test_std::env<>>,
                               test_detail::env_type<index, sender_with_env, local_receiver>>);
    static_assert(
        std::same_as<test_detail::fwd_env<local_env>, test_detail::env_type<index, local_sender, receiver_with_env>>);
}

template <typename T>
auto test_basic_receiver() -> void {
    using index = std::integral_constant<int, 0>;
    struct local_tag {
        static auto name() { return "test_basic_receiver"; }
    };
    struct data {};
    struct err {
        int  value{};
        auto operator==(const err&) const -> bool = default;
    };
    struct local_sender {
        local_tag t{};
        data      d{};
    };
    struct local_receiver {
        T    value{};
        err  error{};
        bool stopped{};

        auto set_value(T val) noexcept { this->value = val; }
        auto set_error(err e) noexcept { this->error = e; }
        auto set_stopped() noexcept { this->stopped = true; }
    };
    struct unstoppable_receiver {
        T value;
    };
    using basic_receiver = test_detail::basic_receiver<local_sender, local_receiver, index>;
    static_assert(test_std::receiver<basic_receiver>);
    static_assert(std::same_as<local_tag, typename basic_receiver::tag_t>);
    static_assert(
        std::same_as<test_detail::state_type<local_sender, local_receiver>, typename basic_receiver::state_t>);
    ASSERT(&basic_receiver::complete == &test_detail::default_impls::complete);

    {
        test_detail::basic_state<local_sender, local_receiver> op(local_sender{}, local_receiver{});
        basic_receiver                                         br{&op};
        static_assert(not requires { test_std::set_value(std::move(br)); });
        static_assert(not requires { test_std::set_value(std::move(br), 42, 1); });
        static_assert(requires { test_std::set_value(std::move(br), 42); });
        static_assert(noexcept(test_std::set_value(std::move(br), 42)));
        ASSERT(op.receiver.value == 0);
        test_std::set_value(std::move(br), 42);
        ASSERT(op.receiver.value == 42);
    }
    {
        test_detail::basic_state<local_sender, local_receiver> op(local_sender{}, local_receiver{});
        basic_receiver                                         br{&op};
        static_assert(not requires { test_std::set_error(std::move(br)); });
        static_assert(not requires { test_std::set_error(std::move(br), 0); });
        static_assert(requires { test_std::set_error(std::move(br), err{42}); });
        static_assert(noexcept(test_std::set_error(std::move(br), err{42})));
        ASSERT(op.receiver.error == err{});
        test_std::set_error(std::move(br), err{42});
        ASSERT(op.receiver.error == err{42});
    }
    {
        test_detail::basic_state<local_sender, local_receiver> op(local_sender{}, local_receiver{});
        basic_receiver                                         br{&op};
        static_assert(requires { test_std::set_stopped(std::move(br)); });
        static_assert(noexcept(test_std::set_stopped(std::move(br))));
        ASSERT(op.receiver.stopped == false);
        test_std::set_stopped(std::move(br));
        ASSERT(op.receiver.stopped == true);
    }
    {
        test_detail::basic_state<local_sender, unstoppable_receiver> op(local_sender{}, unstoppable_receiver{});
        test_detail::basic_receiver<local_sender, unstoppable_receiver, index> br{&op};
        static_assert(not requires { std::move(br).set_stopped(); });
    }
    //-dk:TODO test basic_receiver::get_env
}

auto test_completion_tag() -> void {
    struct no_completion {};

    static_assert(test_detail::completion_tag<test_std::set_error_t>);
    static_assert(test_detail::completion_tag<test_std::set_stopped_t>);
    static_assert(test_detail::completion_tag<test_std::set_value_t>);
    static_assert(not test_detail::completion_tag<int>);
    static_assert(not test_detail::completion_tag<no_completion>);
}

auto test_product_type() -> void {
    struct nm {
        int value{};
        explicit nm(int val) : value(val) {}
        nm(nm&&)                                 = delete;
        nm(const nm&)                            = delete;
        ~nm()                                    = default;
        auto operator=(nm&&) -> nm&              = delete;
        auto operator=(const nm&) -> nm&         = delete;
        auto operator==(const nm&) const -> bool = default;
    };
    auto p0{test_detail::product_type{}};
    static_assert(p0.size() == 0u);
    ASSERT(p0 == p0);

    auto p1{test_detail::product_type{nm(1)}};
    static_assert(p1.size() == 1u);
    ASSERT(p1.get<0>() == nm(1));

    auto p2{test_detail::product_type{nm(1), nm(2)}};
    static_assert(p2.size() == 2u);
    ASSERT(p2.get<0>() == nm(1));
    ASSERT(p2.get<1>() == nm(2));

    auto p3{test_detail::product_type{nm(1), nm(2), nm(3)}};
    static_assert(p3.size() == 3u);
    ASSERT(p3.get<0>() == nm(1));
    ASSERT(p3.get<1>() == nm(2));
    ASSERT(p3.get<2>() == nm(3));

    auto p4{test_detail::product_type{nm(1), nm(2), nm(3), nm(4)}};
    static_assert(p4.size() == 4u);
    ASSERT(p4.get<0>() == nm(1));
    ASSERT(p4.get<1>() == nm(2));
    ASSERT(p4.get<2>() == nm(3));
    ASSERT(p4.get<3>() == nm(4));

    auto p5{test_detail::product_type{nm(1), nm(2), nm(3), nm(4), nm(5)}};
    static_assert(p5.size() == 5u);
    ASSERT(p5.get<0>() == nm(1));
    ASSERT(p5.get<1>() == nm(2));
    ASSERT(p5.get<2>() == nm(3));
    ASSERT(p5.get<3>() == nm(4));
    ASSERT(p5.get<4>() == nm(5));

    test_detail::product_type prod{1, true, 'c'};
    static_assert(test_detail::is_product_type_c<decltype(prod)>);
    static_assert(3u == std::tuple_size<decltype(prod)>::value);
    static_assert(std::same_as<int, std::tuple_element<0u, decltype(prod)>::type>);
    static_assert(std::same_as<bool, std::tuple_element<1u, decltype(prod)>::type>);
    static_assert(std::same_as<char, std::tuple_element<2u, decltype(prod)>::type>);
    auto&& [i, b, c] = prod;
    test::use(i, b, c);

    struct derived : decltype(prod) {};
    static_assert(3u == std::tuple_size<derived>::value);
    static_assert(std::same_as<int, std::tuple_element<0u, derived>::type>);
    static_assert(std::same_as<bool, std::tuple_element<1u, derived>::type>);
    static_assert(std::same_as<char, std::tuple_element<2u, derived>::type>);
    derived d{1, true, 'c'};
    auto&& [di, db, dc] = d;
    assert(di == d.get<0>());
    assert(db == d.get<1>());
    assert(dc == d.get<2>());
    test::use(di, db, dc);
}
auto test_connect_all() -> void {
    static_assert(test_std::operation_state<operation_state<receiver>>);
    {
        test_detail::basic_state state{sender0{}, receiver{}};
        auto                     product{test_detail::connect_all(&state, sender0{}, std::index_sequence<>{})};
        ASSERT(product.size() == 0);
        test::use(product);
    }
    {
        using csender0 = const sender0;
        test_detail::basic_state state{csender0{}, receiver{}};
        auto                     product{test_detail::connect_all(&state, csender0{}, std::index_sequence<>{})};
        ASSERT(product.size() == 0);
        test::use(product);
    }
    {
        static_assert(requires { sender1{}.connect(receiver{}); });
        static_assert(requires { test_std::connect(sender1{}, receiver{}); });
        test_detail::basic_state state{sender1{}, receiver{}};
        auto                     product{test_detail::connect_all(&state, sender1{}, std::index_sequence<0>{})};
        ASSERT(product.size() == 1);
        test::use(product);
    }
    {
        using csender1 = const sender1;
        csender1 s{};
        static_assert(requires { s.connect(receiver{}); });
        static_assert(requires { test_std::connect(s, receiver{}); });
        test_detail::basic_state state{csender1{}, receiver{}};
        auto                     product{test_detail::connect_all(&state, csender1{}, std::index_sequence<0>{})};
        ASSERT(product.size() == 1);
        test::use(product);
    }
    {
        static_assert(requires { sender2{}.connect(receiver{}); });
        static_assert(requires { test_std::connect(sender2{}, receiver{}); });
        test_detail::basic_state state{sender2{}, receiver{}};
        auto                     product{test_detail::connect_all(&state, sender2{}, std::index_sequence<0, 1>{})};
        ASSERT(product.size() == 2);
        test::use(product);
    }
    {
        using csender2 = const sender2;
        csender2 s{};
        static_assert(requires { s.connect(receiver{}); });
        static_assert(requires { test_std::connect(s, receiver{}); });
        test_detail::basic_state state{csender2{}, receiver{}};
        auto                     product{test_detail::connect_all(&state, csender2{}, std::index_sequence<0, 1>{})};
        ASSERT(product.size() == 2);
        test::use(product);
    }
    {
        static_assert(requires { sender3{}.connect(receiver{}); });
        static_assert(requires { test_std::connect(sender3{}, receiver{}); });
        test_detail::basic_state state{sender3{}, receiver{}};
        auto                     product{test_detail::connect_all(&state, sender3{}, std::index_sequence<0, 1, 2>{})};
        ASSERT(product.size() == 3);
        test::use(product);
    }
    {
        using csender3 = const sender3;
        csender3 s{};
        static_assert(requires { s.connect(receiver{}); });
        static_assert(requires { test_std::connect(s, receiver{}); });
        test_detail::basic_state state{csender3{}, receiver{}};
        auto                     product{test_detail::connect_all(&state, csender3{}, std::index_sequence<0, 1, 2>{})};
        ASSERT(product.size() == 3);
        test::use(product);
    }
    {
        static_assert(requires { sender4{}.connect(receiver{}); });
        static_assert(requires { test_std::connect(sender4{}, receiver{}); });
        test_detail::basic_state state{sender4{}, receiver{}};
        auto product{test_detail::connect_all(&state, sender4{}, std::index_sequence<0, 1, 2, 3>{})};
        ASSERT(product.size() == 4);
        test::use(product);
    }
    {
        using csender4 = const sender4;
        csender4 s{};
        static_assert(requires { s.connect(receiver{}); });
        static_assert(requires { test_std::connect(s, receiver{}); });
        test_detail::basic_state state{csender4{}, receiver{}};
        auto product{test_detail::connect_all(&state, csender4{}, std::index_sequence<0, 1, 2, 3>{})};
        ASSERT(product.size() == 4);
        test::use(product);
    }

    //-dk: TODO test connect_all
}

auto test_connect_all_result() -> void {
    {
        test_detail::basic_state state{sender0{}, receiver{}};
        static_assert(
            std::same_as<decltype(test_detail::connect_all(&state, sender0{}, std::index_sequence<0, 1, 2, 3>{})),
                         test_detail::connect_all_result<sender0, receiver>>);
    }
    {
        test_detail::basic_state state{sender1{}, receiver{}};
        static_assert(
            std::same_as<decltype(test_detail::connect_all(&state, sender1{}, std::index_sequence<0, 1, 2, 3>{})),
                         test_detail::connect_all_result<sender1&&, receiver>>);
    }
    {
        test_detail::basic_state state{sender2{}, receiver{}};
        static_assert(
            std::same_as<decltype(test_detail::connect_all(&state, sender2{}, std::index_sequence<0, 1, 2, 3>{})),
                         test_detail::connect_all_result<sender2&&, receiver>>);
    }
    {
        test_detail::basic_state state{sender3{}, receiver{}};
        static_assert(
            std::same_as<decltype(test_detail::connect_all(&state, sender3{}, std::index_sequence<0, 1, 2, 3>{})),
                         test_detail::connect_all_result<sender3&&, receiver>>);
    }
    {
        test_detail::basic_state state{sender4{}, receiver{}};
        static_assert(
            std::same_as<decltype(test_detail::connect_all(&state, sender4{}, std::index_sequence<0, 1, 2, 3>{})),
                         test_detail::connect_all_result<sender4&&, receiver>>);
    }
}

auto test_basic_operation() -> void {
    int                          data{};
    test_detail::basic_operation op{
        sender4{
            tag{}, &data, sender0{tag{}, &data}, sender0{tag{}, &data}, sender0{tag{}, &data}, sender0{tag{}, &data}},
        receiver{}};
    static_assert(test_std::operation_state<decltype(op)>);
    static_assert(requires { typename decltype(op)::tag_t; });
    test_std::start(op);
    ASSERT(data == 4);
}

auto test_completion_signatures_for() -> void {
    struct arg {};
    struct local_env {};
    struct bad_env {};
    struct sender {
        using sender_concept = test_std::sender_t;
        using empty_env_sigs = test_std::completion_signatures<test_std::set_value_t(arg)>;
        using env_sigs       = test_std::completion_signatures<test_std::set_value_t(arg, arg)>;

        auto get_completion_signatures(const test_std::env<>&) -> empty_env_sigs { return {}; }
        auto get_completion_signatures(const local_env&) -> env_sigs { return {}; }
    };

    static_assert(test_std::sender_in<sender, test_std::env<>>);
    static_assert(test_std::sender_in<sender, local_env>);
    static_assert(not test_std::sender_in<sender, bad_env>);

#if 0
        //-dk:TODO restore completion_signatures_for tests
        static_assert(std::same_as<
            test_detail::completion_signatures_for<sender, test_std::env<>>,
            sender::empty_env_sigs
        >);
        static_assert(std::same_as<
            test_detail::completion_signatures_for<sender, local_env>,
            sender::env_sigs
        >);
#endif
    static_assert(
        not test_detail::valid_completion_signatures<test_detail::no_completion_signatures_defined_in_sender>);
    static_assert(std::same_as<test_detail::completion_signatures_for<sender, bad_env>,
                               test_detail::no_completion_signatures_defined_in_sender>);
}

struct basic_sender_tag {
    template <typename>
    struct state {
        using operation_state_concept = test_std::operation_state_t;
        auto start() & noexcept -> void {}
    };
    struct sender {
        using sender_concept        = test_std::sender_t;
        using completion_signatures = test_std::completion_signatures<>;
        template <test_std::receiver Receiver>
        auto connect(Receiver) noexcept -> state<Receiver> {
            return {};
        }
    };
    auto transform_sender(auto&&...) noexcept { return sender{}; }
};

struct data {};
struct tagged_sender : test_detail::product_type<basic_sender_tag, data, sender0> {
    using sender_concept = test_std::sender_t;
};
} // namespace
namespace std {
template <>
struct tuple_size<tagged_sender> : ::std::integral_constant<std::size_t, 3> {};
template <::std::size_t I>
struct tuple_element<I, tagged_sender> {
    using type = ::std::decay_t<decltype(::std::declval<tagged_sender>().template get<I>())>;
};
} // namespace std
namespace {
auto test_basic_sender() -> void {
    struct local_env {};

    {
        auto&& [a, b, c] = tagged_sender{basic_sender_tag{}, data{}, sender0{}};
        test::use(a);
        test::use(b);
        test::use(c);
        static_assert(std::same_as<basic_sender_tag, std::remove_cvref_t<decltype(a)>>);
    }

    static_assert(test_std::sender<basic_sender_tag::sender>);
    static_assert(test_std::sender_in<basic_sender_tag::sender>);
    static_assert(test_std::sender_in<basic_sender_tag::sender, local_env>);
    static_assert(test_std::operation_state<basic_sender_tag::state<receiver>>);
    static_assert(test_std::sender<tagged_sender>);
    static_assert(std::same_as<basic_sender_tag, test_std::tag_of_t<tagged_sender>>);
    static_assert(
        std::same_as<basic_sender_tag::sender,
                     decltype(test_std::transform_sender(test_std::default_domain{}, tagged_sender{}, local_env{}))>);

    using basic_sender = test_detail::basic_sender<basic_sender_tag, data, sender0>;
    static_assert(test_std::sender<basic_sender>);

    basic_sender        bs{basic_sender_tag{}, data{}, sender0{}};
    const basic_sender& cbs{bs};
    test::use(cbs);

    auto&& [tag, data, children] = test_detail::get_sender_data(bs);
    test::use(tag, data, children);
    static_assert(std::same_as<basic_sender_tag, std::remove_cvref_t<decltype(tag)>>);

    static_assert(std::same_as<basic_sender_tag, test_std::tag_of_t<basic_sender>>);
    static_assert(
        std::same_as<basic_sender_tag::sender,
                     decltype(test_std::transform_sender(test_std::default_domain{}, basic_sender{}, local_env{}))>);
    static_assert(test_std::sender_in<basic_sender>);
#if 0
        //-dk:TODO restore completion_sigatures_for test
        static_assert(std::same_as<
            basic_sender_tag::sender::completion_signatures,
            test_detail::completion_signatures_for<basic_sender, env>
        >);
        static_assert(std::same_as<
            basic_sender_tag::sender::completion_signatures,
            test_detail::completion_signatures_for<basic_sender, env>
        >);
#endif

    auto ge{test_std::get_env(bs)};
    test::use(ge);
    static_assert(std::same_as<test_detail::fwd_env<sender0::env>, decltype(ge)>);

    auto op{test_std::connect(bs, receiver{})};
    test::use(op);
#if 0
        static_assert(std::same_as<
            basic_sender_tag::sender::completion_signatures,
            decltype(bs.get_completion_signatures(env{}))
        >);
        static_assert(std::same_as<
            basic_sender_tag::sender::completion_signatures,
            decltype(cbs.get_completion_signatures(env{}))
        >);
        static_assert(std::same_as<
            basic_sender_tag::sender::completion_signatures,
            decltype(basic_sender{ basic_sender_tag{}, data{}, sender0 {} }
                .get_completion_signatures(env{}))
        >);
#endif
    static_assert(std::same_as<std::index_sequence_for<sender0>, basic_sender::indices_for>);
}

template <typename T>
auto test_make_sender() -> void {
    {
        auto sender{test_detail::make_sender(tag{}, {})};
        static_assert(test_std::sender<decltype(sender)>);
    }
    {
        auto sender{test_detail::make_sender(tag{}, int{17})};
        static_assert(test_std::sender<decltype(sender)>);
        static_assert(std::same_as<test_detail::basic_sender<tag, int>, decltype(sender)>);
    }
    {
        static_assert(not requires { test_detail::make_sender(tag{}, int{17}, T()); });
    }
    {
        auto sender{test_detail::make_sender(tag{}, int{17}, sender0{})};
        static_assert(test_std::sender<decltype(sender)>);
        static_assert(std::same_as<test_detail::basic_sender<tag, int, sender0>, decltype(sender)>);
    }
}

template <typename>
struct property {
    struct data {
        int  value{};
        auto operator==(const data&) const -> bool = default;
    };
};

struct write_env_env {
    struct base {};
    int  value{};
    auto query(const property<base>&) const -> property<base>::data { return {this->value}; }
};
struct write_env_added {
    int value{};
    struct added {};
    auto query(const property<added>&) const noexcept -> property<added>::data { return {this->value}; }
};

struct write_env_receiver {
    using receiver_concept = test_std::receiver_t;

    bool* result{nullptr};

    auto get_env() const noexcept -> write_env_env { return {42}; }
    auto set_value(bool value) && noexcept -> void { *this->result = value; }
};

struct write_env_sender {
    using sender_concept        = test_std::sender_t;
    using completion_signatures = test_std::completion_signatures<test_std::set_value_t(bool)>;
    template <typename Receiver>
    struct state {
        using operation_state_concept = test_std::operation_state_t;
        std::remove_cvref_t<Receiver> receiver;

        auto start() & noexcept -> void {
            using base_property  = property<write_env_env::base>;
            using added_property = property<write_env_added::added>;
            bool result{false};

            if constexpr (requires {
                              test_std::get_env(receiver).query(base_property{});
                              test_std::get_env(receiver).query(added_property{});
                          }) {
                result = (base_property::data{42} == test_std::get_env(receiver).query(base_property{})) &&
                         (added_property::data{43} == test_std::get_env(receiver).query(added_property{}));
            }

            test_std::set_value(::std::move(receiver), result);
        }
    };

    template <typename Receiver>
    auto connect(Receiver&& receiver) noexcept -> state<Receiver> {
        return {std::forward<Receiver>(receiver)};
    }
};

auto test_write_env() -> void {
    static_assert(test_std::sender<write_env_sender>);
    static_assert(test_std::receiver<write_env_receiver>);
    static_assert(test_detail::queryable<write_env_added>);
    auto plain_op(test_std::connect(write_env_sender{}, write_env_receiver{}));
    static_assert(std::same_as<write_env_env, decltype(test_std::get_env(write_env_receiver{}))>);
    static_assert(std::same_as<write_env_env, decltype(test_std::get_env(plain_op.receiver))>);
    using base_property = property<write_env_env::base>;
    ASSERT(base_property::data{42} == test_std::get_env(plain_op.receiver).query(base_property{}));

    auto we_sender{test_std::write_env(write_env_sender{}, write_env_added{43})};

    static_assert(test_std::sender_in<write_env_sender>);
    static_assert(std::same_as<test_std::completion_signatures<test_std::set_value_t(bool)>,
                               decltype(test_std::get_completion_signatures(write_env_sender{}, write_env_env{}))>);

    static_assert(std::same_as<test_detail::completion_signatures_for<decltype(we_sender), write_env_env>,
                               test_std::completion_signatures<test_std::set_value_t(bool)>>);
    static_assert(std::same_as<test_detail::completion_signatures_for<decltype(we_sender), test_std::env<>>,
                               test_std::completion_signatures<test_std::set_value_t(bool)>>);
    static_assert(std::same_as<test_detail::completion_signatures_for<decltype(we_sender)&, test_std::env<>>,
                               test_std::completion_signatures<test_std::set_value_t(bool)>>);
    static_assert(test_std::sender_in<decltype(we_sender)>);
    static_assert(std::same_as<test_std::completion_signatures<test_std::set_value_t(bool)>,
                               decltype(test_std::get_completion_signatures(we_sender, write_env_env{}))>);

    static_assert(test_std::sender<decltype(we_sender)>);
    static_assert(std::same_as<test_std::write_env_t, test_std::tag_of_t<decltype(we_sender)>>);

    bool has_both_properties{false};
    ASSERT(not has_both_properties);
    auto we_op{test_std::connect(we_sender, write_env_receiver{&has_both_properties})};
    test_std::start(we_op);
    ASSERT(has_both_properties);
    test::use(we_op);
}

template <typename... T>
struct child_sender : test_detail::product_type<T...> {};
auto test_child_type() -> void {
    static_assert(std::same_as<int&&, test_detail::child_type<child_sender<bool, char, int, double>>>);
    static_assert(std::same_as<int&, test_detail::child_type<child_sender<bool, char, int, double>&>>);
    static_assert(std::same_as<const int&, test_detail::child_type<const child_sender<bool, char, int, double>&>>);
    static_assert(std::same_as<int&&, test_detail::child_type<child_sender<bool, char, int, double>, 0>>);
    static_assert(std::same_as<double&&, test_detail::child_type<child_sender<bool, char, int, double>, 1>>);
}
} // namespace

TEST(exec_snd_expos) {
    test_fwd_env();
    test_make_env();
    test_join_env();
    test_sched_attrs();
    test_sched_env();
    test_completion_domain();
    test_query_with_default();
    test_get_domain_early();
    test_get_domain_late();
    test_default_impls();
    test_impls_for();
    test_state_type();
    test_basic_state();
    test_indices_for();
    test_valid_specialization();
    test_env_type();
    test_basic_receiver<int>();
    test_completion_tag();
    test_product_type();
    test_connect_all();
    test_connect_all_result();
    test_basic_operation();
    test_completion_signatures_for();
    test_basic_sender();
    test_make_sender<int>();
    test_write_env();
    test_child_type();
}
