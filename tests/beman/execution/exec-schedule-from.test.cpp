// src/beman/execution/tests/exec-schedule-from.test.cpp            -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <concepts>
#include <test/execution.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
import beman.execution.detail;
#else
#include <beman/execution/detail/schedule_from.hpp>
#include <beman/execution/execution.hpp>
#endif

// ----------------------------------------------------------------------------

namespace {
struct non_scheduler {};
struct non_sender {};

struct scheduler {
    struct env {
        auto query(const test_std::get_completion_scheduler_t<test_std::set_value_t>&) const noexcept -> scheduler {
            return {};
        }
    };
    struct sender {
        template <typename Receiver>
        struct state {
            using operation_state_concept = test_std::operation_state_t;
            std::remove_cvref_t<Receiver> receiver;
            auto start() & noexcept -> void { test_std::set_value(::std::move(this->receiver)); }
        };
        using sender_concept        = test_std::sender_t;
        using completion_signatures = test_std::completion_signatures<test_std::set_value_t()>;
        auto get_env() const noexcept -> env { return {}; }
        template <test_std::receiver Receiver>
        auto connect(Receiver&& receiver) -> state<Receiver> {
            return {std::forward<Receiver>(receiver)};
        }
    };
    using scheduler_concept = test_std::scheduler_t;
    auto schedule() -> sender { return {}; }
    auto operator==(const scheduler&) const -> bool = default;
};
struct sender {
    using sender_concept        = test_std::sender_t;
    using completion_signatures = test_std::completion_signatures<test_std::set_value_t()>;

    template <typename Receiver>
    struct state {
        using operation_state_concept = test_std::operation_state_t;
        std::remove_cvref_t<Receiver> receiver;
        auto                          start() & noexcept -> void { test_std::set_value(::std::move(this->receiver)); }
    };
    template <test_std::receiver Receiver>
    auto connect(Receiver&& receiver) -> state<Receiver> {
        return {std::forward<Receiver>(receiver)};
    }
};

template <bool Expect, typename Scheduler, typename Sender>
auto test_constraints(Scheduler&& scheduler, Sender&& sender) {
    static_assert(Expect == requires { test_std::schedule_from(scheduler, sender); });
    static_assert(Expect == requires {
        test_std::schedule_from(::std::forward<Scheduler>(scheduler), ::std::forward<Sender>(sender));
    });
}

#if 0
}
namespace beman::execution::detail {
template <typename Scheduler, typename Sender, typename Env>
struct completion_signatures_for_impl<
    ::beman::execution::detail::basic_sender<::beman::execution::schedule_from_t, Scheduler, Sender>,
    Env> {
    using scheduler_sender = decltype(::beman::execution::schedule(::std::declval<Scheduler>()));
    template <typename... E>
    using as_set_error = ::beman::execution::completion_signatures<::beman::execution::set_error_t(E)...>;
    using type         = ::beman::execution::detail::meta::combine<
                decltype(::beman::execution::get_completion_signatures(::std::declval<Sender>(), ::std::declval<Env>())),
                ::beman::execution::error_types_of_t<scheduler_sender, Env, as_set_error>,
                ::beman::execution::completion_signatures<::beman::execution::set_error_t(
            ::std::exception_ptr)> //-dk:TODO this one should be deduced
                >;
};
}
namespace {
#endif
template <typename Scheduler, typename Sender>
auto test_use(Scheduler&& scheduler, Sender&& sender) {
    auto s{test_std::schedule_from(::std::forward<Scheduler>(scheduler), ::std::forward<Sender>(sender))};

    static_assert(test_std::sender<decltype(s)>);
    using stype = std::remove_cvref_t<decltype(s)>;
    using type = typename test_detail::completion_signatures_for_impl<stype, test_std::env<>>::type;
    type t = 17;
    //std::same_as<type, test_std::completion_signatures<test_std::set_value_t()>>;
    //auto sig{test_std::get_completion_signatures(s, test_std::env{})};
    //static_assert(std::same_as<decltype(sig), test_std::completion_signatures<test_std::set_value_t()>>);
    //-dk:TODO test_std::sync_wait(std::move(s));
}
} // namespace

TEST(exec_schedule_from) {
    static_assert(std::same_as<const test_std::schedule_from_t, decltype(test_std::schedule_from)>);
    static_assert(not test_std::scheduler<non_scheduler>);
    static_assert(test_std::scheduler<scheduler>);
    static_assert(not test_std::sender<non_sender>);
    static_assert(test_std::sender<sender>);

    test_constraints<false>(non_scheduler{}, non_sender{});
    test_constraints<false>(non_scheduler{}, sender{});
    test_constraints<false>(scheduler{}, non_sender{});
    test_constraints<true>(scheduler{}, sender{});

    test_use(scheduler{}, sender{});
}
