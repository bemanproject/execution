// include/beman/execution/detail/notify.hpp                        -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_NOTIFY
#define INCLUDED_BEMAN_EXECUTION_DETAIL_NOTIFY

#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <mutex>
#include <utility>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail.basic_sender;
import beman.execution.detail.completion_signatures;
import beman.execution.detail.completion_signatures_for;
import beman.execution.detail.default_impls;
import beman.execution.detail.immovable;
import beman.execution.detail.impls_for;
import beman.execution.detail.make_sender;
import beman.execution.detail.set_value;
#else
#include <beman/execution/detail/immovable.hpp>
#include <beman/execution/detail/make_sender.hpp>
#endif

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
struct notify_t;
class notifier : ::beman::execution::detail::immovable {
  public:
    auto complete() -> void {
        ::std::unique_lock kerberos(this->lock);
        this->completed = true;
        while (this->head) {
            auto* next{::std::exchange(this->head, this->head->next)};
            kerberos.unlock();
            next->complete();
            kerberos.lock();
        }
    }

  private:
    friend struct impls_for<::beman::execution::detail::notify_t>;
    struct base : ::beman::execution::detail::virtual_immovable {
        base*        next{};
        virtual auto complete() -> void = 0;
    };
    std::mutex lock;
    bool       completed{};
    base*      head{};

    auto add(base* b) -> bool {
        ::std::lock_guard kerberbos(this->lock);
        if (this->completed)
            return false;
        b->next = std::exchange(this->head, b);
        return true;
    }
};

struct notify_t {
    auto operator()(::beman::execution::detail::notifier& n) const {
        return ::beman::execution::detail::make_sender(*this, &n);
    }

    template <typename, typename...>
    static consteval auto get_completion_signatures() {
        return ::beman::execution::completion_signatures<::beman::execution::set_value_t()>();
    }
};
inline constexpr ::beman::execution::detail::notify_t notify{};

template <>
struct impls_for<::beman::execution::detail::notify_t> : ::beman::execution::detail::default_impls {
    template <typename Receiver>
    struct state : ::beman::execution::detail::notifier::base {
        ::beman::execution::detail::notifier* n;
        ::std::remove_cvref_t<Receiver>&      receiver{};
        state(::beman::execution::detail::notifier* nn, ::std::remove_cvref_t<Receiver>& rcvr)
            : n(nn), receiver(rcvr) {}
        auto complete() -> void override { ::beman::execution::set_value(::std::move(this->receiver)); }
    };
    static constexpr auto get_state{[]<typename Sender, typename Receiver>(Sender&& sender, Receiver&& receiver) {
        ::beman::execution::detail::notifier* n{sender.template get<1>()};
        return state<Receiver>(n, receiver);
    }};
    static constexpr auto start{[](auto& state, auto&) noexcept -> void {
        if (not state.n->add(&state)) {
            state.complete();
        }
    }};
};
} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_NOTIFY
