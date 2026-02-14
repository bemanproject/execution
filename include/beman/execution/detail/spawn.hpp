// include/beman/execution/detail/spawn.hpp                           -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_SPAWN
#define INCLUDED_BEMAN_EXECUTION_DETAIL_SPAWN

#include <beman/execution/detail/common.hpp>
#include <beman/execution/detail/spawn_get_allocator.hpp>
#include <beman/execution/detail/scope_token.hpp>
#include <beman/execution/detail/env.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/receiver.hpp>
#include <beman/execution/detail/connect_result_t.hpp>
#include <beman/execution/detail/connect.hpp>
#include <beman/execution/detail/start.hpp>
#include <beman/execution/detail/write_env.hpp>
#include <memory>
#include <utility>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
struct spawn_t {
    struct state_base {
        virtual ~state_base()                    = default;
        virtual auto complete() noexcept -> void = 0;
    };

    struct receiver {
        using receiver_concept = ::beman::execution::receiver_t;
        state_base* state{};

        auto set_value() && noexcept -> void { this->state->complete(); }
        auto set_stopped() && noexcept -> void { this->state->complete(); }
    };

    template <typename Alloc, ::beman::execution::scope_token Tok, ::beman::execution::sender Sndr>
    struct state : state_base {
        using op_t     = ::beman::execution::connect_result_t<Sndr, receiver>;
        using assoc_t  = ::std::remove_cvref_t<decltype(::std::declval<Tok&>().try_associate())>;
        using alloc_t  = typename ::std::allocator_traits<Alloc>::template rebind_alloc<state>;
        using traits_t = ::std::allocator_traits<alloc_t>;

        state(Alloc a, Sndr&& sndr, Tok tok)
            : alloc(a),
              op(::beman::execution::connect(::std::forward<Sndr>(sndr), receiver{this})),
              assoc(tok.try_associate()) {
            if (this->assoc) {
                ::beman::execution::start(this->op);
            } else {
                this->destroy();
            }
        }
        auto complete() noexcept -> void override {
            assoc_t _ = ::std::move(this->assoc);
            this->destroy();
        }
        auto destroy() noexcept -> void {
            alloc_t all(this->alloc);
            traits_t::destroy(all, this);
            traits_t::deallocate(all, this, 1);
        }

        alloc_t alloc;
        op_t    op;
        assoc_t assoc;
    };

    template <::beman::execution::sender Sender, ::beman::execution::scope_token Token, typename Env>
    auto operator()(Sender&& sender, Token&& tok, Env&& env) const {
        auto new_sender{tok.wrap(::std::forward<Sender>(sender))};
        auto [all, senv] = ::beman::execution::detail::spawn_get_allocator(new_sender, env);

        using sender_t = decltype(::beman::execution::write_env(::std::move(new_sender), senv));
        using state_t  = state<decltype(all), Token, sender_t>;
        using alloc_t  = typename ::std::allocator_traits<decltype(all)>::template rebind_alloc<state_t>;
        using traits_t = ::std::allocator_traits<alloc_t>;
        alloc_t  alloc(all);
        state_t* op{traits_t::allocate(alloc, 1u)};
        traits_t::construct(alloc, op, all, ::beman::execution::write_env(::std::move(new_sender), senv), tok);
    }
    template <::beman::execution::sender Sender, ::beman::execution::scope_token Token>
    auto operator()(Sender&& sender, Token&& token) const {
        return (*this)(::std::forward<Sender>(sender), ::std::forward<Token>(token), ::beman::execution::env<>{});
    }
};
} // namespace beman::execution::detail

namespace beman::execution {
using spawn_t = ::beman::execution::detail::spawn_t;
inline constexpr spawn_t spawn{};
} // namespace beman::execution

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_SPAWN
