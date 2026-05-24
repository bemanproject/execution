// include/beman/execution/detail/task/task_scheduler.hpp                  -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_TASK_SCHEDULER
#define INCLUDED_BEMAN_EXECUTION_DETAIL_TASK_SCHEDULER

#include <beman/execution/detail/common.hpp>
#include <beman/execution/detail/task/infallible_scheduler.hpp>
#include <beman/execution/detail/task/poly.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <memory>
#include <new>
#include <type_traits>
#include <utility>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail.completion_signatures;
import beman.execution.detail.connect;
import beman.execution.detail.env;
import beman.execution.detail.get_completion_scheduler;
import beman.execution.detail.get_env;
import beman.execution.detail.operation_state;
import beman.execution.detail.receiver;
import beman.execution.detail.schedule;
import beman.execution.detail.scheduler;
import beman.execution.detail.scheduler_tag;
import beman.execution.detail.sender;
import beman.execution.detail.set_value;
import beman.execution.detail.start;
#else
#include <beman/execution/detail/completion_signatures.hpp>
#include <beman/execution/detail/connect.hpp>
#include <beman/execution/detail/env.hpp>
#include <beman/execution/detail/get_completion_scheduler.hpp>
#include <beman/execution/detail/get_env.hpp>
#include <beman/execution/detail/operation_state.hpp>
#include <beman/execution/detail/receiver.hpp>
#include <beman/execution/detail/schedule.hpp>
#include <beman/execution/detail/scheduler.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/set_value.hpp>
#include <beman/execution/detail/start.hpp>
#endif

// ----------------------------------------------------------------------------

namespace beman::execution::detail {

/*!
 * \brief Type-erasing scheduler
 * \headerfile beman/execution/task.hpp <beman/execution/task.hpp>
 *
 * The class `task_scheduler` is used to type-erase any infallible scheduler class.
 */
class task_scheduler {
    struct state_base {
        virtual ~state_base()         = default;
        virtual void complete_value() = 0;
    };

    struct inner_state {
        struct receiver {
            using receiver_concept = ::beman::execution::receiver_tag;
            state_base* state;
            void        set_value() && noexcept { this->state->complete_value(); }
        };
        static_assert(::beman::execution::receiver<receiver>);

        struct base {
            virtual ~base()      = default;
            virtual void start() = 0;
        };
        template <::beman::execution::sender Sender>
        struct concrete : base {
            using state_t =
                decltype(::beman::execution::connect(::std::declval<Sender>(), ::std::declval<receiver>()));
            state_t state;

            template <::beman::execution::sender S>
            concrete(S&& s, state_base* b) : state(::beman::execution::connect(::std::forward<S>(s), receiver{b})) {}
            void start() override { ::beman::execution::start(state); }
        };

        ::beman::execution::detail::poly<base, 16u * sizeof(void*)> state;

        template <::beman::execution::sender S>
        inner_state(S&& s, state_base* b) : state(static_cast<concrete<S>*>(nullptr), ::std::forward<S>(s), b) {}

        void start() { this->state->start(); }
    };

    template <::beman::execution::receiver Receiver>
    struct state : state_base {
        using operation_state_concept = ::beman::execution::operation_state_tag;
        ::std::remove_cvref_t<Receiver> receiver;
        inner_state                     s;

        template <::beman::execution::receiver R, typename PS>
        state(R&& r, PS& ps) : receiver(::std::forward<R>(r)), s(ps->connect(this)) {}
        void start() & noexcept { this->s.start(); }
        void complete_value() override { ::beman::execution::set_value(::std::move(this->receiver)); }
    };

    class sender;
    class env {
        friend class sender;

      private:
        const sender* sndr;
        explicit env(const sender* s) : sndr(s) {}

      public:
        task_scheduler
        query(const ::beman::execution::get_completion_scheduler_t<::beman::execution::set_value_t>&) const noexcept {
            return this->sndr->inner_sender->get_completion_scheduler();
        }
    };

    class sender {
        friend class env;

      private:
        struct base {
            virtual ~base()                                         = default;
            virtual base*          move(void*)                      = 0;
            virtual base*          clone(void*) const               = 0;
            virtual inner_state    connect(state_base*)             = 0;
            virtual task_scheduler get_completion_scheduler() const = 0;
        };
        template <::beman::execution::scheduler Scheduler>
        struct concrete : base {
            using sender_type = decltype(::beman::execution::schedule(::std::declval<Scheduler>()));
            sender_type sender;

            template <::beman::execution::scheduler S>
            explicit concrete(S&& s) : sender(::beman::execution::schedule(::std::forward<S>(s))) {}
            base*          move(void* buffer) override { return new (buffer) concrete(::std::move(*this)); }
            base*          clone(void* buffer) const override { return new (buffer) concrete(*this); }
            inner_state    connect(state_base* b) override { return inner_state(::std::move(sender), b); }
            task_scheduler get_completion_scheduler() const override {
                return task_scheduler(::beman::execution::get_completion_scheduler<::beman::execution::set_value_t>(
                    ::beman::execution::get_env(this->sender)));
            }
        };
        ::beman::execution::detail::poly<base, 4u * sizeof(void*)> inner_sender;

      public:
        using sender_concept        = ::beman::execution::sender_tag;
        using completion_signatures = ::beman::execution::completion_signatures<::beman::execution::set_value_t()>;

        template <typename...>
        static consteval auto get_completion_signatures() noexcept -> completion_signatures {
            return {};
        }

        template <::beman::execution::scheduler S>
        explicit sender(S&& s) : inner_sender(static_cast<concrete<S>*>(nullptr), ::std::forward<S>(s)) {}
        sender(sender&&)      = default;
        sender(const sender&) = default;

        template <::beman::execution::receiver R>
        auto connect(R&& r) -> state<R> {
            return state<R>(::std::forward<R>(r), this->inner_sender);
        }

        auto get_env() const noexcept -> env { return env(this); }
    };

    struct base {
        virtual ~base()                          = default;
        virtual sender schedule()                = 0;
        virtual base*  move(void*)               = 0;
        virtual base*  clone(void*) const        = 0;
        virtual bool   equals(const base*) const = 0;
    };
    template <::beman::execution::scheduler Scheduler>
    struct concrete : base {
        Scheduler scheduler;

        template <typename S>
            requires ::beman::execution::scheduler<::std::remove_cvref_t<S>>
        explicit concrete(S&& s) : scheduler(::std::forward<S>(s)) {}
        sender schedule() override { return sender(this->scheduler); }
        base*  move(void* buffer) override { return new (buffer) concrete(::std::move(*this)); }
        base*  clone(void* buffer) const override { return new (buffer) concrete(*this); }
        bool   equals(const base* o) const override {
            auto other{dynamic_cast<const concrete*>(o)};
            return other ? this->scheduler == other->scheduler : false;
        }
    };

    ::beman::execution::detail::poly<base, 4u * sizeof(void*)> scheduler;

  public:
    using scheduler_concept = ::beman::execution::scheduler_tag;

    template <typename S, typename Allocator = ::std::allocator<void>>
        requires(not ::std::same_as<task_scheduler, ::std::remove_cvref_t<S>>) &&
                ::beman::execution::scheduler<::std::remove_cvref_t<S>> &&
                ::beman::execution::detail::infallible_scheduler<::std::remove_cvref_t<S>, ::beman::execution::env<>>
    explicit task_scheduler(S&& s, Allocator = {})
        : scheduler(static_cast<concrete<::std::decay_t<S>>*>(nullptr), ::std::forward<S>(s)) {}
    task_scheduler(task_scheduler&&)      = default;
    task_scheduler(const task_scheduler&) = default;
    template <typename Allocator>
    task_scheduler(const task_scheduler& other, Allocator) : scheduler(other.scheduler) {}
    auto operator=(const task_scheduler&) -> task_scheduler& = default;
    ~task_scheduler()                                        = default;

    auto schedule() -> sender { return this->scheduler->schedule(); }
    auto operator==(const task_scheduler&) const -> bool = default;

    template <typename Sched>
        requires(not ::std::same_as<task_scheduler, Sched>) && ::beman::execution::scheduler<Sched>
    auto operator==(const Sched& other) const -> bool {
        return *this == task_scheduler(other);
    }
};
static_assert(::beman::execution::scheduler<task_scheduler>);

} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif
