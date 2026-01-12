// include/beman/execution/detail/affine_on.hpp                       -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_AFFINE_ON
#define INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_AFFINE_ON

#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/transform_sender.hpp>
#include <beman/execution/detail/schedule_from.hpp>
#include <beman/execution/detail/get_scheduler.hpp>
#include <beman/execution/detail/env.hpp>
#include <beman/execution/detail/get_domain_early.hpp>
#include <beman/execution/detail/make_sender.hpp>
#include <beman/execution/detail/sender_adaptor_closure.hpp>
#include <beman/execution/detail/sender_adaptor.hpp>
#include <beman/execution/detail/completion_signatures.hpp>
#include <beman/execution/detail/completion_signatures_of_t.hpp>
#include <beman/execution/detail/completion_signatures_for.hpp>
#include <beman/execution/detail/env.hpp>
#include <beman/execution/detail/forward_like.hpp>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
    struct affine_on_t: ::beman::execution::sender_adaptor_closure<affine_on_t> {
        template <::beman::execution::sender Sender>
        auto operator()(Sender&& sender) const {
            return ::beman::execution::detail::transform_sender(
                ::beman::execution::detail::get_domain_early(sender),
                ::beman::execution::detail::make_sender(
                    *this, ::beman::execution::env<>{}, ::std::forward<Sender>(sender)));
        }
        auto operator()() const {
            return ::beman::execution::detail::sender_adaptor{*this};
        }

        template <::beman::execution::sender Sender, typename Env>
        static auto transform_sender(Sender&& sender, Env const& env) {
            auto&[tag, data, child] = sender;
            return beman::execution::schedule_from(
                ::beman::execution::get_scheduler(env),
                ::beman::execution::detail::forward_like<Sender>(child)
            );
        }
    };
}

namespace beman::execution {
    using beman::execution::detail::affine_on_t;
    inline constexpr affine_on_t affine_on{};
}

// ----------------------------------------------------------------------------

#endif
