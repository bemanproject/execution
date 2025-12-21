// examples/tst-repeat_effect_until.hpp                               -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// ----------------------------------------------------------------------------

#ifndef INCLUDED_EXAMPLES_TST_REPEAT_EFFECT_UNTIL
#define INCLUDED_EXAMPLES_TST_REPEAT_EFFECT_UNTIL

#include <beman/execution/execution.hpp>
#include <optional>
#include <type_traits>
#include <utility>

// ----------------------------------------------------------------------------

namespace tst {
    namespace ex = beman::execution;

    template <ex::sender Sndr, ex::receiver Rcvr>
    struct connector {
        decltype(ex::connect(::std::declval<Sndr>(), ::std::declval<Rcvr>())) op;
        connector(auto sndr, auto rcvr)
            : op(ex::connect(::std::move(sndr), ::std::move(rcvr))) {}
        
        auto start() & noexcept -> void { ex::start(this->op); }
    };

    inline constexpr struct repeat_effect_unilt_t {
        template <ex::sender Child, typename Fun>
        struct sender {
            using sender_concept        = ex::sender_t;
            using completion_signatures
                = ex::completion_signatures<ex::set_value_t(), ex::set_error_t(std::exception_ptr), ex::set_stopped_t()>;
            
            template <ex::receiver Receiver>
            struct state {
                using operation_state_concept = ex::operation_state_t;
                struct own_receiver {
                    using receiver_concept = ex::receiver_t;
                    state* s;
                    auto set_value() && noexcept -> void {
                        static_assert(ex::receiver<own_receiver>);
                        this->s->next();
                    }
                    auto set_error(std::exception_ptr error) && noexcept -> void {
                        ex::set_error(::std::move(this->s->receiver), std::move(error));
                    }
                    auto set_stopped() && noexcept -> void {
                        ex::set_stopped(::std::move(this->s->receiver));
                    }
                };

                std::remove_cvref_t<Child> child;
                std::remove_cvref_t<Fun> fun;
                std::remove_cvref_t<Receiver> receiver;
                std::optional<tst::connector<std::remove_cvref_t<Child>, own_receiver>> child_op;

                auto start() & noexcept -> void {
                    static_assert(ex::operation_state<state>);
                    this->run_one();
                }
                auto run_one() & noexcept -> void {
                    this->child_op.emplace(this->child, own_receiver{this});
                    this->child_op->start();
                }
                auto next() & noexcept -> void {
                    if (this->fun()) {
                        ex::set_value(::std::move(this->receiver));
                    } else {
                        this->run_one();
                    }
                }
            };

            std::remove_cvref_t<Child> child;
            std::remove_cvref_t<Fun> fun;

            template <ex::receiver Receiver>
            auto connect(Receiver&& receiver) const& noexcept -> state<Receiver> {
                static_assert(ex::sender<sender>);
                return state<Receiver>(this->child, this->fun, ::std::forward<Receiver>(receiver));
            }
        };
        template <ex::sender Child, typename Pred>
        auto operator()(Child&& child, Pred&& pred) const -> sender<Child, Pred> {
            return {::std::forward<Child>(child), ::std::forward<Pred>(pred)};
        }
    } repeat_effect_until{};
}

// ----------------------------------------------------------------------------

#endif
