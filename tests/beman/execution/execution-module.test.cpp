// tests/beman/execution/execution-module.test.cpp                  -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <test/execution.hpp>
import beman.execution;

// ----------------------------------------------------------------------------

namespace {
#ifdef TEST_THIS_CODE
// [exec.getcomplsigs], completion signatures
template <typename S>
using value_types_of_t = test_std::value_types_of_t<S>;
template <typename S>
using error_types_of_t = test_std::error_types_of_t<S>;
#endif
} // namespace

TEST(execution_modules) {
#if 0
    //-dk:TOD enable execution policies
    test::use_type<test_std::sequenced_policy>();
    test::use_type<test_std::parallel_policy>();
    test::use_type<test_std::parallel_unsequenced_policy>();
    test::use_type<test_std::unsequenced_policy>();

    test::use(test_std::seq);
    test::use(test_std::par);
    test::use(test_std::par_unseq);
    test::use(test_std::unseq);
#endif

    // [exec.queries], queries
    test::use_type<test_std::forwarding_query_t>();
    test::use_type<test_std::get_allocator_t>();
    test::use_type<test_std::get_stop_token_t>();

    test::use_type<test_std::forwarding_query_t>();
    test::use_type<test_std::get_allocator_t>();
    test::use_type<test_std::get_stop_token_t>();

    test::use_template<test_std::stop_token_of_t>();

    test::use_type<test_std::get_domain_t>();
    test::use_type<test_std::get_scheduler_t>();
    test::use_type<test_std::get_delegation_scheduler_t>();
    //-dk:TODO test::use_type<test_std::get_forward_progress_guarantee_t>();
    test::use_template<test_std::get_completion_scheduler_t>();

    test::use(test_std::get_domain);
    test::use(test_std::get_scheduler);
    test::use(test_std::get_delegation_scheduler);
    //-dk:TODO test::use_type<test_std::forward_progress_guarantee>();
    //-dk:TODO test::use(test_std::get_forward_progress_guarantee);
    test::use(test_std::get_completion_scheduler<test_std::set_value_t>);

    test::use_type<test_std::env<>>();
    test::use_type<test_std::get_env_t>();
    test::use(test_std::get_env);
    test::use_template<test_std::env_of_t>();

    // [exec.domain.default], execution_domains
    test::use_type<test_std::default_domain>();

    // [exec.sched], schedulers
    test::use_type<test_std::scheduler_t>();
    static_assert(not test_std::scheduler<int>);

    // [exec.recv], receivers
    test::use_type<test_std::receiver_t>();
    static_assert(not test_std::receiver<int>);
    static_assert(not test_std::receiver_of<int, test_std::completion_signatures<>>);

    test::use_type<test_std::set_value_t>();
    test::use_type<test_std::set_error_t>();
    test::use_type<test_std::set_stopped_t>();

    test::use(test_std::set_value);
    test::use(test_std::set_error);
    test::use(test_std::set_stopped);

    // [exec.opstate], operation states
    test::use_type<test_std::operation_state_t>();
    static_assert(not test_std::operation_state<int>);
    test::use_type<test_std::start_t>();
    test::use(test_std::start);

    // [exec.snd], senders
    test::use_type<test_std::sender_t>();
    static_assert(not test_std::sender<int>);
    static_assert(not test_std::sender_in<int>);
    //-dk:TODO static_assert(not test_std::sender_to<int, int>);

    // [exec.getcomplsigs], completion signatures
    test::use_type<test_std::get_completion_signatures_t>();
    test::use(test_std::get_completion_signatures);

    test::use_template<test_std::completion_signatures_of_t>();
    static_assert(not test_std::sends_stopped<decltype(test_std::just())>);
    test::use_template<test_std::tag_of_t>();

    // [exec.snd.transform], sender transformations
    test_std::transform_sender(test_std::default_domain{}, test_std::just());

    // [exec.snd.transform.env], environment transformations
    //-dk:TODO test_std::transform_env(test_std::default_domain{}, test_std::just(), test_std::env<>{});

    // [exec.snd.apply], sender algorithm application
    //-dk:TODO test_std::apply_sender(test_std::default_domain{}, test_std::just_t{}, test_std::just());

    // [exec.connect], the connect sender algorithm
    test::use_type<test_std::connect_t>();
    test::use(test_std::connect);
    test::use_template<test_std::connect_result_t>();

    // [exec.factories], sender factories
    test::use_type<test_std::just_t>();
    test::use_type<test_std::just_error_t>();
    test::use_type<test_std::just_stopped_t>();
    test::use_type<test_std::schedule_t>();

    test::use(test_std::just);
    test::use(test_std::just_error);
    test::use(test_std::just_stopped);
    test::use(test_std::schedule);
    test::use(test_std::read_env);

    test::use_template<test_std::schedule_result_t>();

    // [exec.adapt], sender adaptors
    test::use_template<test_std::sender_adaptor_closure>();

    test::use_type<test_std::starts_on_t>();
    test::use_type<test_std::continues_on_t>();
    test::use_type<test_std::on_t>();
    test::use_type<test_std::schedule_from_t>();
    test::use_type<test_std::then_t>();
    test::use_type<test_std::upon_error_t>();
    test::use_type<test_std::upon_stopped_t>();
    test::use_type<test_std::let_value_t>();
    test::use_type<test_std::let_error_t>();
    test::use_type<test_std::let_stopped_t>();
    test::use_type<test_std::bulk_t>();
    test::use_type<test_std::split_t>();
    test::use_type<test_std::when_all_t>();
    test::use_type<test_std::when_all_with_variant_t>();
    test::use_type<test_std::into_variant_t>();
    //-dk:TODO test::use_type<test_std::stopped_as_optional_t>();
    //-dk:TODO test::use_type<test_std::stopped_as_error_t>();

    test::use(test_std::starts_on);
    test::use(test_std::continues_on);
    test::use(test_std::on);
    test::use(test_std::schedule_from);
    test::use(test_std::then);
    test::use(test_std::upon_error);
    test::use(test_std::upon_stopped);
    test::use(test_std::let_value);
    test::use(test_std::let_error);
    test::use(test_std::let_stopped);
    test::use(test_std::bulk);
    test::use(test_std::split);
    test::use(test_std::when_all);
    test::use(test_std::when_all_with_variant);
    test::use(test_std::into_variant);
    //-dk:TODO test::use(test_std::stopped_as_optional);
    //-dk:TODO test::use(test_std::stopped_as_error);

    // [exec.util.cmplsig]
    test::use_template<test_std::completion_signatures>();

    // [exec.util.cmplsig.trans]
    //-dk:TODO template <typename S> using transform_completion_signatures =
    // test_std::transform_completion_signatures<S>; -dk:TODO template <typename S> using
    // transform_completion_signatures_of = test_std::transform_completion_signatures_of<S>;

    // [exec.run.loop], run_loop
    test::use_type<test_std::run_loop>();

    // [exec.consumers], consumers
    test::use_type<test_std::sync_wait_t>();
    //-dk:TODO test::use_type<test_std_this_thread::sync_wait_with_variant_t>();

    test::use(test_std::sync_wait);
    //-dk:TODO test::use(test_std_this_thread::sync_wait_with_variant);

    // [exec.as.awaitable]
    test::use_type<test_std::as_awaitable_t>();
    test::use(test_std::as_awaitable);

    // [exec.with.awaitable.senders]
    test::use_template<test_std::with_awaitable_senders>();
}
