// include/beman/execution/detail/task_scheduler.hpp                  -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_TASK_SCHEDULER
#define INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_TASK_SCHEDULER

// ----------------------------------------------------------------------------

#include <beman/execution/execution.hpp>
#include <memory>
#include <utility>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {

class task_scheduler {
  private:
    struct operation_base {
        virtual ~operation_base() = default;
        virtual void start() noexcept = 0;
    };

    template <typename Operation>
    struct operation_model final : operation_base {
        Operation op_;

        template <typename O>
        explicit operation_model(O&& op) : op_(std::forward<O>(op)) {}

        void start() noexcept override { ::beman::execution::start(op_); }
    };

    struct scheduler_base {
        virtual ~scheduler_base() = default;
        virtual std::unique_ptr<scheduler_base> clone() const = 0;
        virtual std::unique_ptr<operation_base>
        connect(void (*set_value)(void*) noexcept, void (*set_stopped)(void*) noexcept, void* receiver_data) = 0;
        virtual bool equals(const scheduler_base& other) const noexcept = 0;
    };

    template <typename Scheduler>
    struct scheduler_model final : scheduler_base {
        Scheduler sched_;

        explicit scheduler_model(Scheduler sched) : sched_(std::move(sched)) {}

        std::unique_ptr<scheduler_base> clone() const override {
            return std::make_unique<scheduler_model>(sched_);
        }

        bool equals(const scheduler_base& other) const noexcept override {
            if (auto* p = dynamic_cast<const scheduler_model*>(&other)) {
                return sched_ == p->sched_;
            }
            return false;
        }

        std::unique_ptr<operation_base>
        connect(void (*set_value)(void*) noexcept, void (*set_stopped)(void*) noexcept, void* receiver_data) override {
            struct inline_receiver {
                void (*set_value_)(void*) noexcept;
                void (*set_stopped_)(void*) noexcept;
                void* data_;

                using receiver_concept = ::beman::execution::receiver_t;

                void set_value() && noexcept { set_value_(data_); }
                void set_stopped() && noexcept { set_stopped_(data_); }
            };

            auto sender = ::beman::execution::schedule(sched_);
            auto op = ::beman::execution::connect(std::move(sender),
                                                   inline_receiver{set_value, set_stopped, receiver_data});
            using op_t = decltype(op);
            return std::make_unique<operation_model<op_t>>(std::move(op));
        }
    };

    std::unique_ptr<scheduler_base> impl_;

  public:
    struct env {
        scheduler_base* sched_;
        auto query(::beman::execution::get_completion_scheduler_t<::beman::execution::set_value_t>) const noexcept {
            return task_scheduler(sched_);
        }
    };
    class sender {
      public:
        using sender_concept = ::beman::execution::sender_t;

        template <typename Env>
        auto get_completion_signatures(Env&&) const noexcept {
            using stop_token_type = ::beman::execution::stop_token_of_t<Env>;
            if constexpr (::beman::execution::unstoppable_token<stop_token_type>) {
                return ::beman::execution::completion_signatures<
                    ::beman::execution::set_value_t()>{};
            } else {
                return ::beman::execution::completion_signatures<
                    ::beman::execution::set_value_t(),
                    ::beman::execution::set_stopped_t()>{};
            }
        }

      private:
        friend class task_scheduler;
        scheduler_base* sched_;

        explicit sender(scheduler_base* sched) : sched_(sched) {}

      public:
        template <typename Receiver>
        class operation {
          private:
            Receiver rcvr_;
            std::unique_ptr<operation_base> op_;

            static void do_set_value(void* data) noexcept {
                auto* self = static_cast<Receiver*>(data);
                ::beman::execution::set_value(std::move(*self));
            }

            static void do_set_stopped(void* data) noexcept {
                auto* self = static_cast<Receiver*>(data);
                ::beman::execution::set_stopped(std::move(*self));
            }

          public:
            using operation_state_concept = ::beman::execution::operation_state_t;

            template <typename R>
            operation(scheduler_base* sched, R&& rcvr)
                : rcvr_(std::forward<R>(rcvr)),
                  op_(sched->connect(&do_set_value, &do_set_stopped, &rcvr_)) {}

            void start() & noexcept { op_->start(); }
        };

        template <::beman::execution::receiver Receiver>
        auto connect(Receiver&& rcvr) && {
            return operation<std::decay_t<Receiver>>(sched_, std::forward<Receiver>(rcvr));
        }
        auto get_env() const noexcept { return env{sched_}; }
    };

    using scheduler_concept = ::beman::execution::scheduler_t;

    template <typename Scheduler>
        requires(!std::same_as<std::decay_t<Scheduler>, task_scheduler>
            && ::beman::execution::scheduler<std::decay_t<Scheduler>>
        )
    explicit task_scheduler(Scheduler&& sched)
        : impl_(std::make_unique<scheduler_model<std::decay_t<Scheduler>>>(std::forward<Scheduler>(sched))) {}
    explicit task_scheduler(scheduler_base* impl) : impl_(impl->clone()) {}

    task_scheduler(const task_scheduler& other)
        : impl_(other.impl_->clone()) {}

    task_scheduler(task_scheduler&&) noexcept = default;

    task_scheduler& operator=(const task_scheduler& other) {
        this->impl_ = other.impl_->clone();
        return *this;
    }

    task_scheduler& operator=(task_scheduler&& other) noexcept {
        ::std::swap(this->impl_, other.impl_);
        return *this;
    }

    ~task_scheduler() = default;

    sender schedule() { return sender{impl_.get()}; }

    bool operator==(const task_scheduler& other) const noexcept {
        return impl_->equals(*other.impl_);
    }
};

//static_assert(::beman::execution::scheduler<task_scheduler>);
static_assert(::beman::execution::sender<task_scheduler::sender>);


} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif
