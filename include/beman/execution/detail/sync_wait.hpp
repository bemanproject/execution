// include/beman/execution/detail/sync_wait.hpp                     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_SYNC_WAIT
#define INCLUDED_BEMAN_EXECUTION_DETAIL_SYNC_WAIT

#include <beman/execution/detail/common.hpp>
#include <beman/execution/detail/as_except_ptr.hpp>
#include <beman/execution/detail/sender_in.hpp>
#include <beman/execution/detail/get_domain_early.hpp>
#include <beman/execution/detail/get_scheduler.hpp>
#include <beman/execution/detail/get_delegation_scheduler.hpp>
#include <beman/execution/detail/apply_sender.hpp>
#include <beman/execution/detail/connect.hpp>
#include <beman/execution/detail/start.hpp>
#include <beman/execution/detail/run_loop.hpp>
#include <beman/execution/detail/receiver.hpp>
#include <beman/execution/detail/sender_in.hpp>
#include <beman/execution/detail/value_types_of_t.hpp>
#include <beman/execution/detail/decayed_tuple.hpp>
#include <exception>
#include <optional>
#include <utility>
#include <type_traits>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
BEMAN_EXECUTION_EXPORT struct sync_wait_env { // dk:TODO detail export
    ::beman::execution::run_loop* loop{};

    auto query(::beman::execution::get_scheduler_t) const noexcept { return this->loop->get_scheduler(); }
    auto query(::beman::execution::get_delegation_scheduler_t) const noexcept { return this->loop->get_scheduler(); }
};

BEMAN_EXECUTION_EXPORT template <
    ::beman::execution::sender_in<::beman::execution::detail::sync_wait_env> Sender> // dk:TODO detail export
using sync_wait_result_type =
    ::std::optional<::beman::execution::value_types_of_t<Sender,
                                                         ::beman::execution::detail::sync_wait_env,
                                                         ::beman::execution::detail::decayed_tuple,
                                                         ::std::type_identity_t>>;

BEMAN_EXECUTION_EXPORT template <typename Sender> // dk:TODO detail export
struct sync_wait_state {
    ::beman::execution::run_loop loop{};
    ::std::exception_ptr         error{};

    ::beman::execution::detail::sync_wait_result_type<Sender> result{};
};

BEMAN_EXECUTION_EXPORT template <typename Sender> // dk:TODO detail export
struct sync_wait_receiver {
    using receiver_concept = ::beman::execution::receiver_t;

    ::beman::execution::detail::sync_wait_state<Sender>* state{};

    template <typename Error>
    auto set_error(Error&& error) && noexcept -> void {
        this->state->error = ::beman::execution::detail::as_except_ptr(::std::forward<Error>(error));
        this->state->loop.finish();
    }
    auto set_stopped() && noexcept -> void { this->state->loop.finish(); }
    template <typename... Args>
    auto set_value(Args&&... args) && noexcept -> void {
        try {
            this->state->result.emplace(::std::forward<Args>(args)...);
        } catch (...) {
            this->state->error = ::std::current_exception();
        }
        this->state->loop.finish();
    }

    auto get_env() const noexcept -> ::beman::execution::detail::sync_wait_env {
        return ::beman::execution::detail::sync_wait_env{&this->state->loop};
    }
};

struct sync_wait_t {
    template <typename Sender>
    auto apply_sender(Sender&& sender) const {
        ::beman::execution::detail::sync_wait_state<Sender> state;
        auto op{::beman::execution::connect(::std::forward<Sender>(sender),
                                            ::beman::execution::detail::sync_wait_receiver<Sender>{&state})};
        ::beman::execution::start(op);

        state.loop.run();
        if (state.error) {
            ::std::rethrow_exception(state.error);
        }
        return ::std::move(state.result);
    }

    template <::beman::execution::sender_in<::beman::execution::detail::sync_wait_env> Sender>
        requires requires(Sender&& sender, const sync_wait_t& self) {
            typename ::beman::execution::detail::sync_wait_result_type<Sender>;
            {
                ::beman::execution::apply_sender(
                    ::beman::execution::detail::get_domain_early(std::forward<Sender>(sender)),
                    self,
                    ::std::forward<Sender>(sender))
            } -> ::std::same_as<::beman::execution::detail::sync_wait_result_type<Sender>>;
        }
    auto operator()(Sender&& sender) const {
        auto domain{::beman::execution::detail::get_domain_early(sender)};
        return ::beman::execution::apply_sender(domain, *this, ::std::forward<Sender>(sender));
    }
};
} // namespace beman::execution::detail

namespace beman::execution {
BEMAN_EXECUTION_EXPORT using sync_wait_t = ::beman::execution::detail::sync_wait_t;
/*!
 * \brief <code>sync_wait(_sender_)</code> starts <code>_sender_</code> and waits for its completion.
 * \headerfile beman/execution/execution.hpp <beman/execution/execution.hpp>
 *
 * \details
 * `sync_wait` is a callable object of type `sync_wait_t`. Invoking
 * <code>sync_wait(_sender_)</code> starts <code>_sender_</code> and
 * waits for its completion. This involves a few steps:
 * 1. A <code>run_loop</code> is created to provide a scheduler.
 * 2. The <code>_sender_</code> is `connect`ed to a receiver capturing
 *     the results and providing an environment with access to the
 *     `run_loop`'s scheduler.
 * 3. The operation state returned from `connect` is `start`ed.
 * 4. The `run_loop` is run to process any work scheduled.
 *
 * Once the <code>_sender_</code> completes, the result is provided by `sync_wait`:
 * - If the <code>_sender_</code> completes with <code>set_value(_arg_...)</code>, `sync_wait` returns
 *     an <code>std::optional<std::tuple<_Arg_...>></code> containing the results
 *     <code>_arg_...</code>.
 * - If the <code>_sender_</code> completes with `set_stopped()`, `sync_wait` returns a
 *    disengaged <code>std::optional<std::tuple<_Arg_...>></code>.
 * - If the <code>_sender_</code> completes with
 *    <code>set_error(_error_)</code>, `sync_wait` throw <code>_error_</code> or rethrows the exception if
 * <code>_error_</code> is an <code>std::exception_ptr</code>.
 *
 * <h4>Usage</h4>
 * <pre>
 * sync_wait(<i>sender</i>...)
 * </pre>
 *
 * <h4>Example</h4>
 *
 * The use of <code>sync_wait(_sender_)</code> is in `main`
 * to synchronously wait for the completion of the asynchronous work
 * of the program represented by <code>_sender_</code>.
 *
 * <pre example="doc-sync_wait.cpp">
 * #include <beman/execution/execution.hpp>
 * #include <cassert>
 *
 * int main() {
 *     auto result = ex::sync_wait(ex::just(17));
 *     assert(result);
 *     assert(*result == std::tuple(17));
 * }
 * </pre>
 */
BEMAN_EXECUTION_EXPORT inline constexpr ::beman::execution::sync_wait_t sync_wait{};
} // namespace beman::execution

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_SYNC_WAIT
