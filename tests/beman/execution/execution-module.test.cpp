// tests/beman/execution/execution-module.test.cpp                  -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <test/execution.hpp>
#if 1
#include <beman/execution/execution.hpp>
#else
import beman_execution;
#endif

// ----------------------------------------------------------------------------

namespace {
#ifdef TEST_THIS_CODE
// [exec.getcomplsigs], completion signatures
template <typename S>
using value_types_of_t = test_stdex::value_types_of_t<S>;
template <typename S>
using error_types_of_t = test_stdex::error_types_of_t<S>;
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

#ifdef TEST_THIS_CODE
    // [exec.queries], queries
    test::use_type<test_std::forwarding_query_t>();
    test::use_type<test_std::get_allocator_t>();
    test::use_type<test_std::get_stop_token_t>();

    test::use_type<test_std::forwarding_query_t>();
    test::use_type<test_std::get_allocator_t>();
    test::use_type<test_std::get_stop_token_t>();

    test::use_template<test_std::stop_token_of_t>();

    test::use_type<test_stdex::get_domain_t>();
    test::use_type<test_stdex::get_scheduler_t>();
    test::use_type<test_stdex::get_delegation_scheduler_t>();
    //-dk:TODO test::use_type<test_stdex::get_forward_progress_guarantee_t>();
    test::use_template<test_stdex::get_completion_scheduler_t>();

    test::use(test_stdex::get_domain);
    test::use(test_stdex::get_scheduler);
    test::use(test_stdex::get_delegation_scheduler);
    //-dk:TODO test::use_type<test_stdex::forward_progress_guarantee>();
    //-dk:TODO test::use(test_stdex::get_forward_progress_guarantee);
    test::use(test_stdex::get_completion_scheduler<test_stdex::set_value_t>);

    test::use_type<test_stdex::env<>>();
    test::use_type<test_stdex::get_env_t>();
    test::use(test_stdex::get_env);
    test::use_template<test_stdex::env_of_t>();

    // [exec.domain.default], execution_domains
    test::use_type<test_stdex::default_domain>();

    // [exec.sched], schedulers
    test::use_type<test_stdex::scheduler_t>();
    static_assert(not test_stdex::scheduler<int>);

    // [exec.recv], receivers
    test::use_type<test_stdex::receiver_t>();
    static_assert(not test_stdex::receiver<int>);
    static_assert(not test_stdex::receiver_of<int, test_stdex::completion_signatures<>>);

    test::use_type<test_stdex::set_value_t>();
    test::use_type<test_stdex::set_error_t>();
    test::use_type<test_stdex::set_stopped_t>();

    test::use(test_stdex::set_value);
    test::use(test_stdex::set_error);
    test::use(test_stdex::set_stopped);

    // [exec.opstate], operation states
    test::use_type<test_stdex::operation_state_t>();
    static_assert(not test_stdex::operation_state<int>);
    test::use_type<test_stdex::start_t>();
    test::use(test_stdex::start);

    // [exec.snd], senders
    test::use_type<test_stdex::sender_t>();
    static_assert(not test_stdex::sender<int>);
    static_assert(not test_stdex::sender_in<int>);
    //-dk:TODO static_assert(not test_stdex::sender_to<int, int>);

    // [exec.getcomplsigs], completion signatures
    test::use_type<test_stdex::get_completion_signatures_t>();
    test::use(test_stdex::get_completion_signatures);

    test::use_template<test_stdex::completion_signatures_of_t>();
    static_assert(not test_stdex::sends_stopped<decltype(test_stdex::just())>);
    test::use_template<test_stdex::tag_of_t>();

    // [exec.snd.transform], sender transformations
    test_stdex::transform_sender(test_stdex::default_domain{}, test_stdex::just());

    // [exec.snd.transform.env], environment transformations
    //-dk:TODO test_stdex::transform_env(test_stdex::default_domain{}, test_stdex::just(), test_stdex::env<>{});

    // [exec.snd.apply], sender algorithm application
    //-dk:TODO test_stdex::apply_sender(test_stdex::default_domain{}, test_stdex::just_t{}, test_stdex::just());

    // [exec.connect], the connect sender algorithm
    test::use_type<test_stdex::connect_t>();
    test::use(test_stdex::connect);
    test::use_template<test_stdex::connect_result_t>();

    // [exec.factories], sender factories
    test::use_type<test_stdex::just_t>();
    test::use_type<test_stdex::just_error_t>();
    test::use_type<test_stdex::just_stopped_t>();
    test::use_type<test_stdex::schedule_t>();

    test::use(test_stdex::just);
    test::use(test_stdex::just_error);
    test::use(test_stdex::just_stopped);
    test::use(test_stdex::schedule);
    test::use(test_stdex::read_env);

    test::use_template<test_stdex::schedule_result_t>();

    // [exec.adapt], sender adaptors
    test::use_template<test_stdex::sender_adaptor_closure>();

    test::use_type<test_stdex::starts_on_t>();
    test::use_type<test_stdex::continues_on_t>();
    //-dk:TODO test::use_type<test_stdex::on_t>();
    test::use_type<test_stdex::schedule_from_t>();
    test::use_type<test_stdex::then_t>();
    test::use_type<test_stdex::upon_error_t>();
    test::use_type<test_stdex::upon_stopped_t>();
    test::use_type<test_stdex::let_value_t>();
    test::use_type<test_stdex::let_error_t>();
    test::use_type<test_stdex::let_stopped_t>();
    //-dk:TODO test::use_type<test_stdex::bulk_t>();
    //-dk:TODO test::use_type<test_stdex::split_t>();
    test::use_type<test_stdex::when_all_t>();
    test::use_type<test_stdex::when_all_with_variant_t>();
    test::use_type<test_stdex::into_variant_t>();
    //-dk:TODO test::use_type<test_stdex::stopped_as_optional_t>();
    //-dk:TODO test::use_type<test_stdex::stopped_as_error_t>();

    test::use(test_stdex::starts_on);
    test::use(test_stdex::continues_on);
    //-dk:TODO test::use(test_stdex::on);
    test::use(test_stdex::schedule_from);
    test::use(test_stdex::then);
    test::use(test_stdex::upon_error);
    test::use(test_stdex::upon_stopped);
    test::use(test_stdex::let_value);
    test::use(test_stdex::let_error);
    test::use(test_stdex::let_stopped);
    //-dk:TODO test::use(test_stdex::bulk);
    //-dk:TODO test::use(test_stdex::split);
    test::use(test_stdex::when_all);
    test::use(test_stdex::when_all_with_variant);
    test::use(test_stdex::into_variant);
    //-dk:TODO test::use(test_stdex::stopped_as_optional);
    //-dk:TODO test::use(test_stdex::stopped_as_error);

    // [exec.util.cmplsig]
    test::use_template<test_stdex::completion_signatures>();

    // [exec.util.cmplsig.trans]
    //-dk:TODO template <typename S> using transform_completion_signatures =
    // test_stdex::transform_completion_signatures<S>; -dk:TODO template <typename S> using
    // transform_completion_signatures_of = test_stdex::transform_completion_signatures_of<S>;

    // [exec.run.loop], run_loop
    test::use_type<test_stdex::run_loop>();

    // [exec.consumers], consumers
    test::use_type<test_std_this_thread::sync_wait_t>();
    //-dk:TODO test::use_type<test_std_this_thread::sync_wait_with_variant_t>();

    test::use(test_std_this_thread::sync_wait);
    //-dk:TODO test::use(test_std_this_thread::sync_wait_with_variant);

    // [exec.as.awaitable]
    //-dk:TODO test::use_type<test_stdex::as_awaitable_t>();
    //-dk:TODO test::use(test_stdex::as_awaitable);

    // [exec.with.awaitable.senders]
    //-dk:TODO test::use_template<test_stdex::with_awaitable_senders>();
#endif
}
