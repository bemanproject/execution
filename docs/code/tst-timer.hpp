// docs/code/tst-timer.hpp                                            -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_DOCS_CODE_TST_TIMER
#define INCLUDED_DOCS_CODE_TST_TIMER

// ----------------------------------------------------------------------------

#include <chrono>
#include <queue>
#include <thread>
#include "tst-config.hpp"

// ----------------------------------------------------------------------------

namespace tst {
    class timer;

    inline constexpr struct resume_after_t {
        auto operator()(auto token, std::chrono::milliseconds duration) const noexcept {
            return token.resume_after(duration);
        }
    } resume_after{};
}

// ----------------------------------------------------------------------------

class tst::timer {
public:
    class when_done_sender {
    public:
        using sender_concept = tst::ex::sender_t;
        using completion_signatures =
            tst::ex::completion_signatures<tst::ex::set_value_t(),
                                                    tst::ex::set_error_t(std::exception_ptr),
                                                    tst::ex::set_stopped_t()>;
        template <tst::ex::receiver Receiver>
        struct state {
            using operation_state_concept = tst::ex::operation_state_t;
            using scheduler_t = decltype(ex::get_scheduler(ex::get_env(std::declval<Receiver>())));
            struct execute {
                state* s{};
                auto operator()() noexcept -> void {
                    this->s->await_one();
                }
            };
            struct pred {
                state* s{};
                auto operator()() noexcept -> bool { return this->s->object->queue.empty(); }
            };
            using inner_sender = decltype(tst::repeat_effect_until(ex::schedule(std::declval<scheduler_t>()) | ex::then(execute()), pred()));
            tst::timer*        object;
            tst::connector<inner_sender, Receiver> inner_op;

            explicit state(tst::timer* obj, Receiver&& rcvr) noexcept
                : object(obj)
                , inner_op(tst::repeat_effect_until(
                                ex::schedule(ex::get_scheduler(ex::get_env(rcvr))) | ex::then(execute{this}),
                                pred{this}),
                            std::forward<Receiver>(rcvr))
            {
                static_assert(tst::ex::operation_state<state>);
            }

            auto start() & noexcept -> void {
                this->inner_op.start();
            }
            auto await_one() & noexcept -> void {
                this->object->await_one();
            }
        };
        explicit when_done_sender(tst::timer* obj) noexcept: object(obj) {
            static_assert(tst::ex::sender<when_done_sender>);
        }

        template <tst::ex::receiver Receiver>
        auto connect(Receiver&& receiver) const& noexcept -> state<Receiver> {
            return state<Receiver>(this->object, ::std::forward<Receiver>(receiver));
        }
    private:
        tst::timer* object;
    };
    struct base {
        virtual auto complete() noexcept -> void = 0;
    };
    struct resume_after_sender
    {
        using sender_concept = tst::ex::sender_t;
        using completion_signatures =
            tst::ex::completion_signatures<tst::ex::set_value_t()>;
        template <tst::ex::receiver Receiver>
        struct state: base {
            using operation_state_concept = ex::operation_state_t;
            tst::timer*               object;
            std::chrono::milliseconds duration;
            std::remove_cvref_t<Receiver> receiver;
            state(tst::timer* obj,
                  std::chrono::milliseconds dur,
                  Receiver&& rcvr) noexcept
                : object(obj)
                , duration(dur)
                , receiver(std::forward<Receiver>(rcvr)) {
                static_assert(tst::ex::operation_state<state>);
            }
            auto start() & noexcept -> void {
                this->object->add_timer(this->duration, this);
            }
            auto complete() noexcept -> void override {
                tst::ex::set_value(std::move(this->receiver));
            }
        };
        tst::timer* object;
        std::chrono::milliseconds duration;
        template <ex::receiver Receiver>
        auto connect(Receiver&& receiver) const& noexcept -> state<Receiver> {
            static_assert(ex::sender<resume_after_sender>);
            return state<Receiver>(this->object, this->duration, std::forward<Receiver>(receiver));
        }
    };
    class token {
    public:
        explicit token(timer* obj) noexcept: object(obj) {}
        auto resume_after(std::chrono::milliseconds duration) noexcept -> resume_after_sender {
            static_assert(ex::sender<resume_after_sender>);
            return resume_after_sender{this->object, duration};
        }

    private:
        timer* object;
    };

    auto get_token() noexcept -> token { return token(this); }
    auto when_done() noexcept -> when_done_sender { return when_done_sender(this); }

private:
    auto await_one() -> void {
        if (not this->queue.empty()) {
            auto[time, node] = this->queue.top();
            this->queue.pop();
            ::std::this_thread::sleep_until(time);
            node->complete();
        }
    }
    auto add_timer(std::chrono::milliseconds duration, base* node) noexcept -> void {
        auto time_point = std::chrono::system_clock::now() + duration;
        this->queue.emplace(time_point, node);
    }
    std::priority_queue<std::pair<std::chrono::system_clock::time_point, base*>,
                                std::vector<std::pair<std::chrono::system_clock::time_point, base*>>,
                                std::greater<>>
        queue;
};

// ----------------------------------------------------------------------------

#endif
