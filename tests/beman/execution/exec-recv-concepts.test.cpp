// src/beman/execution/tests/exec-recv-concepts.test.cpp            -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <test/execution.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
import beman.execution.detail;
#else
#include <beman/execution/detail/has_completions.hpp>
#include <beman/execution/detail/inlinable_receiver.hpp>
#include <beman/execution/detail/receiver_of.hpp>
#include <beman/execution/detail/valid_completion_for.hpp>
#include <beman/execution.hpp>
#endif

// ----------------------------------------------------------------------------

namespace {
struct arg {};
struct error {};

struct not_a_receiver {
    auto get_env() const noexcept -> test_std::env<> { return {}; }
    auto set_value(auto&&...) && noexcept -> void {}
    auto set_error(auto&&) && noexcept -> void {}
    auto set_stopped() && noexcept -> void {}
};

struct final_receiver final {
    using receiver_concept = test_std::receiver_tag;
};

template <typename... T>
struct value_receiver {
    using receiver_concept = test_std::receiver_tag;
    auto set_value(T...) && noexcept -> void {}
};

template <typename T>
struct error_receiver {
    using receiver_concept = test_std::receiver_tag;
    auto set_error(T) && noexcept -> void {}
};

struct stopped_receiver {
    using receiver_concept = test_std::receiver_tag;
    auto set_stopped() && noexcept -> void {}
};

template <typename Concept = test_std::receiver_tag>
struct multi_receiver {
    using receiver_concept = Concept;

    auto set_value(int) && noexcept -> void {}
    auto set_value(int, arg) && noexcept -> void {}
    auto set_value(arg, arg) && noexcept -> void {}
    auto set_error(error) && noexcept -> void {}
    auto set_stopped() && noexcept -> void {}
};

auto test_valid_completion_for() -> void {
    static_assert(test_detail::valid_completion_for<test_std::set_value_t(), value_receiver<>>);
    static_assert(not test_detail::valid_completion_for<test_std::set_stopped_t(), value_receiver<>>);
    static_assert(not test_detail::valid_completion_for<test_std::set_value_t(arg), value_receiver<>>);
    static_assert(test_detail::valid_completion_for<test_std::set_value_t(arg), value_receiver<arg>>);
    static_assert(not test_detail::valid_completion_for<test_std::set_value_t(int, arg), value_receiver<int>>);
    static_assert(test_detail::valid_completion_for<test_std::set_value_t(int, arg), value_receiver<int, arg>>);
    static_assert(not test_detail::valid_completion_for<test_std::set_error_t(error), value_receiver<error>>);

    static_assert(not test_detail::valid_completion_for<test_std::set_stopped_t(), error_receiver<error>>);
    static_assert(not test_detail::valid_completion_for<test_std::set_value_t(error), error_receiver<error>>);
    static_assert(test_detail::valid_completion_for<test_std::set_error_t(error), error_receiver<error>>);
    static_assert(not test_detail::valid_completion_for<test_std::set_error_t(error), error_receiver<int>>);
    static_assert(not test_detail::valid_completion_for<test_std::set_error_t(int), error_receiver<error>>);

    static_assert(test_detail::valid_completion_for<test_std::set_stopped_t(), stopped_receiver>);
    static_assert(not test_detail::valid_completion_for<test_std::set_value_t(), stopped_receiver>);
}

auto test_has_completions() -> void {
    static_assert(not test_std::receiver<not_a_receiver>);
    static_assert(not test_std::receiver<final_receiver>);
    static_assert(test_std::receiver<value_receiver<int>>);
    static_assert(test_detail::has_completions<value_receiver<int>, test_std::completion_signatures<>>);
    static_assert(test_detail::has_completions<value_receiver<int>,
                                               test_std::completion_signatures<test_std::set_value_t(int)>>);
    static_assert(not test_detail::has_completions<value_receiver<int>,
                                                   test_std::completion_signatures<test_std::set_value_t(int, int)>>);

    static_assert(test_std::receiver<error_receiver<int>>);
    static_assert(test_detail::has_completions<error_receiver<int>,
                                               test_std::completion_signatures<test_std::set_error_t(int)>>);
    static_assert(not test_detail::has_completions<error_receiver<int>,
                                                   test_std::completion_signatures<test_std::set_error_t(error)>>);
    static_assert(test_detail::has_completions<error_receiver<error>,
                                               test_std::completion_signatures<test_std::set_error_t(error)>>);

    static_assert(test_std::receiver<stopped_receiver>);
    static_assert(not test_detail::has_completions<error_receiver<error>,
                                                   test_std::completion_signatures<test_std::set_stopped_t()>>);
    static_assert(
        test_detail::has_completions<stopped_receiver, test_std::completion_signatures<test_std::set_stopped_t()>>);

    static_assert(test_std::receiver<stopped_receiver>);
    static_assert(test_detail::has_completions<multi_receiver<>,
                                               test_std::completion_signatures<test_std::set_value_t(int),
                                                                               test_std::set_value_t(int, arg),
                                                                               test_std::set_value_t(arg, arg),
                                                                               test_std::set_error_t(error),
                                                                               test_std::set_stopped_t()>>);
    static_assert(not test_detail::has_completions<multi_receiver<>,
                                                   test_std::completion_signatures<test_std::set_value_t(int),
                                                                                   test_std::set_value_t(arg, int),
                                                                                   test_std::set_value_t(arg, arg),
                                                                                   test_std::set_error_t(error),
                                                                                   test_std::set_stopped_t()>>);
    static_assert(not test_detail::has_completions<multi_receiver<>,
                                                   test_std::completion_signatures<test_std::set_value_t(int),
                                                                                   test_std::set_value_t(int, arg),
                                                                                   test_std::set_value_t(arg, arg),
                                                                                   test_std::set_error_t(int),
                                                                                   test_std::set_stopped_t()>>);
}

auto test_receiver_of() -> void {
    static_assert(test_std::receiver_of<value_receiver<int>, test_std::completion_signatures<>>);
    static_assert(
        test_std::receiver_of<value_receiver<int>, test_std::completion_signatures<test_std::set_value_t(int)>>);
    static_assert(not test_std::receiver_of<value_receiver<int>,
                                            test_std::completion_signatures<test_std::set_value_t(int, int)>>);

    static_assert(
        test_std::receiver_of<error_receiver<int>, test_std::completion_signatures<test_std::set_error_t(int)>>);
    static_assert(
        not test_std::receiver_of<error_receiver<int>, test_std::completion_signatures<test_std::set_error_t(error)>>);
    static_assert(
        test_std::receiver_of<error_receiver<error>, test_std::completion_signatures<test_std::set_error_t(error)>>);

    static_assert(
        not test_std::receiver_of<error_receiver<error>, test_std::completion_signatures<test_std::set_stopped_t()>>);
    static_assert(test_std::receiver_of<stopped_receiver, test_std::completion_signatures<test_std::set_stopped_t()>>);

    static_assert(test_std::receiver_of<multi_receiver<>,
                                        test_std::completion_signatures<test_std::set_value_t(int),
                                                                        test_std::set_value_t(int, arg),
                                                                        test_std::set_value_t(arg, arg),
                                                                        test_std::set_error_t(error),
                                                                        test_std::set_stopped_t()>>);
    static_assert(not test_std::receiver_of<multi_receiver<int>,
                                            test_std::completion_signatures<test_std::set_value_t(int),
                                                                            test_std::set_value_t(int, arg),
                                                                            test_std::set_value_t(arg, arg),
                                                                            test_std::set_error_t(error),
                                                                            test_std::set_stopped_t()>>);
    static_assert(not test_std::receiver_of<multi_receiver<>,
                                            test_std::completion_signatures<test_std::set_value_t(int),
                                                                            test_std::set_value_t(arg, int),
                                                                            test_std::set_value_t(arg, arg),
                                                                            test_std::set_error_t(error),
                                                                            test_std::set_stopped_t()>>);
    static_assert(not test_std::receiver_of<multi_receiver<>,
                                            test_std::completion_signatures<test_std::set_value_t(int),
                                                                            test_std::set_value_t(int, arg),
                                                                            test_std::set_value_t(arg, arg),
                                                                            test_std::set_error_t(int),
                                                                            test_std::set_stopped_t()>>);
}

template <typename ParentOp, typename Env>
struct test_basic_receiver {
    using receiver_concept = test_std::receiver_tag;

    template <typename ChildOp>
    static auto make_receiver_for(ChildOp* child) noexcept -> test_basic_receiver {
        // `ChildOp` is the first non-static data member of `ParentOp`.
        const auto address = reinterpret_cast<void*>(child);
        // NOTE: Actually, it's still UB. Under the current standard, pointer conversion/arithmetic cannot achieve
        // this. A truly well-defined approach will likely have to wait until proposals like P3407 are adopted into the
        // standard.
        return test_basic_receiver{reinterpret_cast<ParentOp*>(address)};
    }

    template <typename... Args>
    auto set_value(Args&&... args) noexcept -> void {
        test_std::set_value(std::move(parent->get_receiver()), std::forward<Args>(args)...);
    }

    template <typename E>
    auto set_error(E&& e) noexcept -> void {
        test_std::set_error(std::move(parent->get_receiver()), std::forward<E>(e));
    }

    auto set_stopped() noexcept -> void { test_std::set_stopped(std::move(parent->get_receiver())); }

    auto get_env() const noexcept -> Env { return test_std::get_env(parent->get_receiver()); }

    ParentOp* parent;
};

template <typename Sndr, typename Rcvr>
struct test_basic_state {
    using operation_state_concept = test_std::operation_state_tag;

    using receiver_t = test_basic_receiver<test_basic_state, test_std::env_of_t<Rcvr>>;

    using child_op_t = test_std::connect_result_t<Sndr, receiver_t>;

    test_basic_state(Sndr sndr, Rcvr rcvr) : rcvr(std::move(rcvr)) {
        // The child operation state should be created after the receiver is constructed
        ::new (static_cast<void*>(storage)) child_op_t(test_std::connect(std::move(sndr), receiver_t{this}));
    }

    auto start() & noexcept -> void { test_std::start(*::std::launder(reinterpret_cast<child_op_t*>(storage))); }

    auto get_receiver() noexcept -> Rcvr& { return rcvr; }

    alignas(child_op_t) std::byte storage[sizeof(child_op_t)];
    Rcvr rcvr;
};

template <typename Sndr, typename Rcvr>
    requires test_std::inlinable_receiver<Rcvr, test_basic_state<Sndr, Rcvr>>
struct test_basic_state<Sndr, Rcvr> {
    using operation_state_concept = test_std::operation_state_tag;

    using receiver_t = test_basic_receiver<test_basic_state, test_std::env_of_t<Rcvr>>;

    using child_op_t = test_std::connect_result_t<Sndr, receiver_t>;

    test_basic_state(Sndr sndr, const Rcvr&) : child_op(test_std::connect(std::move(sndr), receiver_t{this})) {}

    auto start() & noexcept -> void { test_std::start(child_op); }

    auto get_receiver() noexcept -> Rcvr { return Rcvr::make_receiver_for(this); }

    child_op_t child_op;
};

template <typename Tag, typename Child>
struct test_basic_sender {
    using sender_concept = test_std::sender_tag;
    Tag   tag;
    Child child;

    template <typename, typename... Env>
        requires test_std::sender_in<Child, Env...>
    static consteval auto get_completion_signatures() {
        return test_std::get_completion_signatures<Child, Env...>();
    }

    template <typename Rcvr>
    auto connect(Rcvr rcvr) {
        return test_basic_state<Child, Rcvr>{std::move(child), std::move(rcvr)};
    }
};

struct test_adaptor : test_std::sender_adaptor_closure<test_adaptor> {
    template <typename Sndr>
    auto operator()(Sndr sndr) {
        return test_basic_sender<test_adaptor, Sndr>{*this, std::move(sndr)};
    }
};

struct inlinable_state {};
struct non_inlinable_state {};
struct normal_receiver {
    using receiver_concept = test_std::receiver_tag;
    static auto set_value(const auto&...) noexcept -> void {}
    static auto set_error(const auto&) noexcept -> void {}
    static auto set_stopped() noexcept -> void {}
};

struct inlinable_receiver {
    using receiver_concept = test_std::receiver_tag;
    static auto make_receiver_for(inlinable_state*) noexcept -> inlinable_receiver { return inlinable_receiver{}; }
    static auto make_receiver_for(non_inlinable_state*) noexcept -> normal_receiver { return normal_receiver{}; }
    template <typename Child, typename Rcvr>
    static auto make_receiver_for(test_basic_state<Child, Rcvr>*) noexcept -> inlinable_receiver {
        return inlinable_receiver{};
    }
    static auto set_value(const auto&...) noexcept -> void {}
    static auto set_error(const auto&) noexcept -> void {}
    static auto set_stopped() noexcept -> void {}
};
static_assert(test_std::receiver<inlinable_receiver>);

auto test_inlinable_receiver() -> void {
    static_assert(not test_std::inlinable_receiver<inlinable_receiver, int>);
    static_assert(not test_std::inlinable_receiver<inlinable_receiver, non_inlinable_state>);
    static_assert(test_std::inlinable_receiver<inlinable_receiver, inlinable_state>);

    auto sndr             = test_std::just(42) | test_adaptor{};
    using normal_state    = test_std::connect_result_t<decltype(sndr), normal_receiver>;
    using inlinable_state = test_std::connect_result_t<decltype(sndr), inlinable_receiver>;
    static_assert(test_std::operation_state<normal_state>);
    static_assert(test_std::operation_state<inlinable_state>);

    auto nested_sndr             = sndr | test_adaptor{};
    using nested_normal_state    = test_std::connect_result_t<decltype(nested_sndr), normal_receiver>;
    using nested_inlinable_state = test_std::connect_result_t<decltype(nested_sndr), inlinable_receiver>;
    static_assert(sizeof(normal_state) == sizeof(nested_normal_state));
    static_assert(sizeof(inlinable_state) == sizeof(nested_inlinable_state));
    auto [result] = test_std::sync_wait(nested_sndr).value();
    ASSERT(result == 42);

    auto nested_4_sndr = test_std::just(67) | test_adaptor{} | test_adaptor{} | test_adaptor{} | test_adaptor{};
    using nested_4_normal_state    = test_std::connect_result_t<decltype(nested_4_sndr), normal_receiver>;
    using nested_4_inlinable_state = test_std::connect_result_t<decltype(nested_4_sndr), inlinable_receiver>;
    static_assert(sizeof(normal_state) == sizeof(nested_4_normal_state));
    static_assert(sizeof(inlinable_state) == sizeof(nested_4_inlinable_state));
    auto [result1] = test_std::sync_wait(nested_4_sndr).value();
    ASSERT(result1 == 67);
}
} // namespace

TEST(exec_recv_concepts) {
    test_valid_completion_for();
    test_has_completions();
    test_receiver_of();
    test_inlinable_receiver();
}
