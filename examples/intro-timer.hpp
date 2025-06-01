// examples/intro-timer.hpp                                           -*-C++-*-
// ----------------------------------------------------------------------------
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// ----------------------------------------------------------------------------

#ifndef INCLUDED_EXAMPLES_INTRO_TIMER
#define INCLUDED_EXAMPLES_INTRO_TIMER

#include <beman/execution/execution.hpp>
#include <chrono>
#include <queue>
#include <thread>
#include <tuple>

// ----------------------------------------------------------------------------

namespace intro {
namespace ex = beman::execution;
struct timer;
} // namespace intro

// ----------------------------------------------------------------------------

struct intro::timer {
    struct state_base {
        virtual ~state_base()   = default;
        virtual void complete() = 0;
    };
    template <typename Receiver>
    struct state : state_base {
        using operation_state_concept = ex::operation_state_t;
        timer*                        self;
        std::remove_cvref_t<Receiver> receiver;
        std::chrono::milliseconds     ms;

        template <typename R>
        state(timer* self, R&& receiver, std::chrono::milliseconds ms)
            : state_base(), self(self), receiver(std::forward<R>(receiver)), ms(ms) {}
        void start() & noexcept { self->add(ms, this); }
        void complete() override { ex::set_value(std::move(receiver)); }
    };
    struct sender {
        using sender_concept        = ex::sender_t;
        using completion_signatures = ex::completion_signatures<ex::set_value_t()>;
        timer*                    self;
        std::chrono::milliseconds ms;
        template <typename R>
        state<R> connect(R&& r) {
            return state<R>(self, std::forward<R>(r), ms);
        }
    };
    using time_point = std::chrono::system_clock::time_point;
    using value_type = std::tuple<time_point, state_base*>;
    std::priority_queue<value_type, std::vector<value_type>, std::greater<>> outstanding;

    void add(std::chrono::milliseconds ms, state_base* base) {
        outstanding.emplace(std::chrono::system_clock::now() + ms, base);
    }
    bool run_one() {
        if (outstanding.empty())
            return false;
        auto [time, base] = outstanding.top();
        outstanding.pop();
        auto now{std::chrono::system_clock::now()};
        if (now < time) {
            std::this_thread::sleep_for(time - now);
        }
        base->complete();
        return true;
    }

    template <typename Receiver>
    struct run_state {
        struct recv {
            using receiver_concept = ex::receiver_t;
            run_state* self;

            auto set_value(auto&&...) noexcept -> void { this->self->run_one(); }
            auto set_error(auto&&) noexcept -> void { this->self->run_one(); }
            auto set_stopped() noexcept -> void { this->self->run_one(); }
        };
        using operation_state_concept = ex::operation_state_t;
        using scheduler_t             = decltype(ex::get_delegation_scheduler(ex::get_env(std::declval<Receiver&>())));
        static_assert(ex::receiver<recv>);
        static_assert(ex::scheduler<scheduler_t>);
        static_assert(ex::sender<decltype(ex::schedule(std::declval<scheduler_t>()))>);
        using state_t = decltype(ex::connect(ex::schedule(std::declval<scheduler_t>()), std::declval<recv>()));
        struct state_ctor {
            state_t state;
            template <typename S, typename R>
            state_ctor(S&& sender, R&& receiver)
                : state(ex::connect(std::forward<S>(sender), std::forward<R>(receiver))) {}
        };

        timer*                    self;
        Receiver                  receiver;
        std::optional<state_ctor> state{};

        auto schedule_one() {
            this->state.emplace(ex::schedule(ex::get_delegation_scheduler(ex::get_env(this->receiver))), recv{this});
            ex::start(this->state->state);
        }
        auto run_one() {
            this->state.reset();
            if (this->self->run_one())
                this->schedule_one();
            else
                ex::set_value(std::move(this->receiver));
        }
        auto start() & noexcept -> void { this->schedule_one(); }
    };
    struct run_sender {
        using sender_concept        = ex::sender_t;
        using completion_signatures = ex::completion_signatures<ex::set_value_t()>;

        timer* self;

        template <typename Receiver>
        run_state<std::remove_cvref_t<Receiver>> connect(Receiver&& receiver) {
            return {self, std::forward<Receiver>(receiver)};
        }
    };

    auto run() { return run_sender{this}; }

    template <typename T, typename P>
    auto resume_after(std::chrono::duration<T, P> d) {
        auto ms(std::chrono::duration_cast<std::chrono::milliseconds>(d));
        return sender{this, ms};
    }
};

// ----------------------------------------------------------------------------

#endif
