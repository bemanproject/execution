// include/beman/execution/detail/run_loop.hpp                      -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_RUN_LOOP
#define INCLUDED_BEMAN_EXECUTION_DETAIL_RUN_LOOP

#include <beman/execution/detail/completion_signatures.hpp>
#include <beman/execution/detail/get_completion_scheduler.hpp>
#include <beman/execution/detail/get_env.hpp>
#include <beman/execution/detail/get_stop_token.hpp>
#include <beman/execution/detail/immovable.hpp>
#include <beman/execution/detail/operation_state.hpp>
#include <beman/execution/detail/scheduler.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/set_stopped.hpp>
#include <beman/execution/detail/set_value.hpp>
#include <beman/execution/detail/unstoppable_token.hpp>

#include <exception>
#include <condition_variable>
#include <mutex>
#include <type_traits>
#include <utility>

// ----------------------------------------------------------------------------

namespace beman::execution {
class run_loop {
  private:
    struct scheduler;

    struct env {
        run_loop* loop;

        template <typename Completion>
        auto query(const ::beman::execution::get_completion_scheduler_t<Completion>&) const noexcept -> scheduler {
            return {this->loop};
        }
    };

    struct opstate_base : ::beman::execution::detail::virtual_immovable {
        opstate_base* next{};
        virtual auto  execute() noexcept -> void = 0;
    };

    template <typename Receiver>
    struct opstate : opstate_base {
        using operation_state_concept = ::beman::execution::operation_state_t;

        run_loop* loop;
        Receiver  receiver;

        // NOLINTBEGIN(misc-no-recursion)
        template <typename R>
        opstate(run_loop* l, R&& rcvr) : loop(l), receiver(::std::forward<Receiver>(rcvr)) {}
        auto start() & noexcept -> void { this->loop->push_back(this); }
        // NOLINTEND(misc-no-recursion)
        auto execute() noexcept -> void override {
            using token = decltype(::beman::execution::get_stop_token(::beman::execution::get_env(this->receiver)));
            if constexpr (not ::beman::execution::unstoppable_token<token>) {
                if (::beman::execution::get_stop_token(::beman::execution::get_env(this->receiver)).stop_requested())
                    ::beman::execution::set_stopped(::std::move(this->receiver));
                else
                    ::beman::execution::set_value(::std::move(this->receiver));
            } else
                ::beman::execution::set_value(::std::move(this->receiver));
        }
    };
    struct sender {
        using sender_concept = ::beman::execution::sender_t;
        template <typename Env = ::beman::execution::env<>>
        auto get_completion_signatures(Env&& env) const noexcept {
            if constexpr (::beman::execution::unstoppable_token<decltype(::beman::execution::get_stop_token(env))>)
                return ::beman::execution::completion_signatures<::beman::execution::set_value_t()>{};
            else
                return ::beman::execution::completion_signatures<::beman::execution::set_value_t(),
                                                                 ::beman::execution::set_stopped_t()>{};
        }

        run_loop* loop;

        auto get_env() const noexcept -> env { return {this->loop}; }
        template <typename Receiver>
        auto connect(Receiver&& receiver) noexcept -> opstate<::std::decay_t<Receiver>> {
            return {this->loop, ::std::forward<Receiver>(receiver)};
        }
    };
    struct scheduler {
        using scheduler_concept = ::beman::execution::scheduler_t;

        run_loop* loop;

        auto schedule() noexcept -> sender { return {this->loop}; }
        auto operator==(const scheduler&) const -> bool = default;
    };

    enum class state : unsigned char { starting, running, finishing };

    state                     current_state{state::starting};
    ::std::mutex              mutex{};
    ::std::condition_variable condition{};
    opstate_base*             front{};
    opstate_base*             back{};

    auto push_back(opstate_base* item) noexcept -> void {
        //-dk:TODO run_loop::push_back should really be lock-free
        ::std::lock_guard guard(this->mutex);
        if (auto previous_back{::std::exchange(this->back, item)}) {
            previous_back->next = item;
        } else {
            this->front = item;
            this->condition.notify_one();
        }
    }
    auto pop_front() noexcept -> opstate_base* {
        //-dk:TODO run_loop::pop_front should really be lock-free
        ::std::unique_lock guard(this->mutex);
        this->condition.wait(guard, [this] { return this->front || this->current_state == state::finishing; });
        if (this->front == this->back)
            this->back = nullptr;
        return this->front ? ::std::exchange(this->front, this->front->next) : nullptr;
    }

  public:
    run_loop() noexcept       = default;
    run_loop(const run_loop&) = delete;
    run_loop(run_loop&&)      = delete;
    ~run_loop() {
        ::std::lock_guard guard(this->mutex);
        if (this->front != nullptr || this->current_state == state::running)
            ::std::terminate();
    }
    auto operator=(const run_loop&) -> run_loop& = delete;
    auto operator=(run_loop&&) -> run_loop&      = delete;

    auto get_scheduler() -> scheduler { return {this}; }

    auto run() -> void {
        if (::std::lock_guard guard(this->mutex);
            this->current_state != state::finishing &&
            state::running == ::std::exchange(this->current_state, state::running)) {
            ::std::terminate();
        }

        while (auto* op{this->pop_front()}) {
            op->execute();
        }
    }
    auto finish() -> void {
        {
            ::std::lock_guard guard(this->mutex);
            this->current_state = state::finishing;
        }
        this->condition.notify_one();
    }
};
} // namespace beman::execution

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_RUN_LOOP
