// tests/beman/execution/task_scheduler.test.cpp                         -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <test/execution.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution/execution.hpp>
#endif
#include <beman/execution/detail/task/task_scheduler.hpp>
#include <atomic>
#include <condition_variable>
#include <exception>
#include <latch>
#include <mutex>
#include <optional>
#include <thread>
#include <utility>

// ----------------------------------------------------------------------------

namespace {
void unexpected_call_assert(const char* msg) { ASSERT(nullptr == msg); }

struct thread_context {
    enum class complete : char { success, never };
    struct base {
        base* next{};
        base()                              = default;
        base(base&&)                        = delete;
        base(const base&)                   = delete;
        virtual ~base()                     = default;
        auto         operator=(base&&)      = delete;
        auto         operator=(const base&) = delete;
        virtual void complete()             = 0;
    };

    ::std::mutex              mutex;
    ::std::condition_variable condition;
    bool                      done{false};
    base*                     work{};
    ::std::thread             thread;

    auto get_work() -> base* {
        ::std::unique_lock guard(this->mutex);
        condition.wait(guard, [this] { return this->done || this->work; });
        base* rc{this->work};
        if (rc) {
            this->work = rc->next;
        }
        return rc;
    }

    auto enqueue(base* w) -> void {
        {
            ::std::lock_guard guard(this->mutex);
            w->next    = this->work;
            this->work = w;
        }
        this->condition.notify_one();
    }

    thread_context()
        : thread([this] {
              while (auto w{this->get_work()}) {
                  w->complete();
              }
          }) {}
    thread_context(thread_context&&)      = delete;
    thread_context(const thread_context&) = delete;
    ~thread_context() {
        this->stop();
        this->thread.join();
    }
    auto operator=(thread_context&&) -> thread_context&      = delete;
    auto operator=(const thread_context&) -> thread_context& = delete;

    struct scheduler {
        using scheduler_concept = test_std::scheduler_tag;
        thread_context* context;
        complete        cmpl{complete::success};
        auto            operator==(const scheduler&) const -> bool = default;

        template <test_std::receiver Receiver>
        struct state : base {
            struct stopper {
                state* st;
                auto   operator()() noexcept -> void {
                    auto self{this->st};
                    self->callback.reset();
                    test_std::set_stopped(::std::move(self->receiver));
                }
            };
            using operation_state_concept = test_std::operation_state_tag;
            using token_t                 = decltype(test_std::get_stop_token(test_std::get_env(::std::declval<Receiver>())));
            using callback_t              = test_std::stop_callback_for_t<token_t, stopper>;

            thread_context*                ctxt;
            ::std::remove_cvref_t<Receiver> receiver;
            thread_context::complete       cmpl;
            ::std::optional<callback_t>    callback;

            template <typename R>
            state(auto c, R&& r, thread_context::complete cm) : ctxt(c), receiver(::std::forward<R>(r)), cmpl(cm) {}
            auto start() & noexcept -> void {
                callback.emplace(test_std::get_stop_token(test_std::get_env(this->receiver)), stopper{this});
                if (cmpl != thread_context::complete::never) {
                    this->ctxt->enqueue(this);
                }
            }
            void complete() override {
                this->callback.reset();
                test_std::set_value(::std::move(this->receiver));
            }
        };

        struct env {
            thread_context* ctxt;
            auto query(const test_std::get_completion_scheduler_t<test_std::set_value_t>&) const noexcept -> scheduler {
                return scheduler{ctxt};
            }
        };

        struct sender {
            using sender_concept        = test_std::sender_tag;
            using completion_signatures = test_std::completion_signatures<test_std::set_value_t()>;

            template <typename>
            static consteval auto get_completion_signatures() -> completion_signatures {
                return {};
            }

            thread_context*          ctxt;
            thread_context::complete cmpl;

            template <test_std::receiver Receiver>
            auto connect(Receiver&& receiver) -> state<Receiver> {
                static_assert(test_std::operation_state<state<Receiver>>);
                return state<Receiver>(this->ctxt, ::std::forward<Receiver>(receiver), this->cmpl);
            }
            auto get_env() const noexcept -> env { return {this->ctxt}; }
        };
        static_assert(test_std::sender<sender>);

        auto schedule() noexcept -> sender { return sender{this->context, this->cmpl}; }
    };
    static_assert(test_std::scheduler<scheduler>);

    auto get_scheduler(complete cmpl = complete::success) -> scheduler { return scheduler{this, cmpl}; }

    auto stop() -> void {
        {
            ::std::lock_guard guard(this->mutex);
            this->done = true;
        }
        this->condition.notify_one();
    }
};
static_assert(test_detail::infallible_scheduler<thread_context::scheduler, test_std::env<>>);

enum class stop_result : char { none, success, failure, stopped };

template <typename Token>
struct stop_env {
    Token token;
    auto  query(test_std::get_stop_token_t) const noexcept { return this->token; }
};
template <typename Token>
stop_env(Token&&) -> stop_env<::std::remove_cvref_t<Token>>;

template <typename Token>
struct stop_receiver {
    using receiver_concept = test_std::receiver_tag;
    Token        token;
    stop_result& result;
    ::std::latch* completed{};

    auto get_env() const noexcept { return stop_env{this->token}; }

    auto set_value(auto&&...) && noexcept -> void {
        this->result = stop_result::success;
        if (this->completed) {
            this->completed->count_down();
        }
    }
    auto set_error(auto&&) && noexcept -> void {
        this->result = stop_result::failure;
        if (this->completed) {
            this->completed->count_down();
        }
    }
    auto set_stopped() && noexcept -> void {
        this->result = stop_result::stopped;
        if (this->completed) {
            this->completed->count_down();
        }
    }
};
template <typename Token>
stop_receiver(Token&&, stop_result&, ::std::latch* = nullptr) -> stop_receiver<::std::remove_cvref_t<Token>>;
static_assert(test_std::receiver<stop_receiver<test_std::inplace_stop_token>>);
} // namespace

// ----------------------------------------------------------------------------

TEST(task_scheduler) {
    try {
        static_assert(test_std::scheduler<test_detail::task_scheduler>);

        thread_context ctxt1;
        thread_context ctxt2;

        ASSERT(ctxt1.get_scheduler() == ctxt1.get_scheduler());
        ASSERT(ctxt2.get_scheduler() == ctxt2.get_scheduler());
        ASSERT(ctxt1.get_scheduler() != ctxt2.get_scheduler());

        test_detail::task_scheduler sched1(ctxt1.get_scheduler());
        test_detail::task_scheduler sched2(ctxt2.get_scheduler());
        ASSERT(sched1 == sched1);
        ASSERT(sched2 == sched2);
        ASSERT(sched1 != sched2);

        test_detail::task_scheduler copy(sched1);
        ASSERT(copy == sched1);
        ASSERT(copy != sched2);
        test_detail::task_scheduler move(::std::move(copy));
        ASSERT(move == sched1);
        ASSERT(move != sched2);

        copy = sched2;
        ASSERT(copy == sched2);
        ASSERT(copy != sched1);

        move = ::std::move(copy);
        ASSERT(move == sched2);
        ASSERT(move != sched1);

        ::std::atomic<::std::thread::id> id1{};
        ::std::atomic<::std::thread::id> id2{};
        test_std::sync_wait(test_std::schedule(sched1) | test_std::then([&id1] { id1 = ::std::this_thread::get_id(); }));
        test_std::sync_wait(test_std::schedule(sched2) | test_std::then([&id2] { id2 = ::std::this_thread::get_id(); }));
        ASSERT(id1 != id2);
        test_std::sync_wait(test_std::schedule(test_detail::task_scheduler(sched1)) |
                            test_std::then([&id1] { ASSERT(id1 == ::std::this_thread::get_id()); }));
        test_std::sync_wait(test_std::schedule(test_detail::task_scheduler(sched2)) |
                            test_std::then([&id2] { ASSERT(id2 == ::std::this_thread::get_id()); }));

        {
            test_std::inplace_stop_source source;
            stop_result                   result{stop_result::none};
            auto state{test_std::connect(test_std::schedule(ctxt1.get_scheduler(thread_context::complete::never)),
                                         stop_receiver{source.get_token(), result})};
            ASSERT(result == stop_result::none);
            test_std::start(state);
            ASSERT(result == stop_result::none);
            source.request_stop();
            ASSERT(result == stop_result::stopped);
        }
        {
            ::std::latch completed{1};
            stop_result  result{stop_result::none};
            auto         state{test_std::connect(
                test_std::schedule(test_detail::task_scheduler(ctxt1.get_scheduler(thread_context::complete::success))),
                stop_receiver{test_std::never_stop_token(), result, &completed})};
            ASSERT(result == stop_result::none);
            test_std::start(state);
            completed.wait();
            ASSERT(result == stop_result::success);
        }
    } catch (...) {
        unexpected_call_assert("no exception should escape to main");
    }
}
