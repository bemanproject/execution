// include/beman/execution/detail/parallel_scheduler.hpp            -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_PARALLEL_SCHEDULER
#define INCLUDED_BEMAN_EXECUTION_DETAIL_PARALLEL_SCHEDULER

#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <cstddef>
#include <concepts>
#include <exception>
#include <memory>
#include <optional>
#include <span>
#include <type_traits>
#include <utility>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail.completion_signatures;
import beman.execution.detail.get_completion_scheduler;
import beman.execution.detail.get_forward_progress_guarantee;
import beman.execution.detail.operation_state;
import beman.execution.detail.receiver;
import beman.execution.detail.scheduler;
import beman.execution.detail.scheduler_tag;
import beman.execution.detail.sender;
import beman.execution.detail.set_error;
import beman.execution.detail.set_stopped;
import beman.execution.detail.set_value;
#else
#include <beman/execution/detail/completion_signatures.hpp>
#include <beman/execution/detail/get_completion_scheduler.hpp>
#include <beman/execution/detail/get_forward_progress_guarantee.hpp>
#include <beman/execution/detail/operation_state.hpp>
#include <beman/execution/detail/receiver.hpp>
#include <beman/execution/detail/scheduler.hpp>
#include <beman/execution/detail/scheduler_tag.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/set_error.hpp>
#include <beman/execution/detail/set_stopped.hpp>
#include <beman/execution/detail/set_value.hpp>
#endif

// ----------------------------------------------------------------------------

namespace beman::execution::system_context_replaceability {

struct receiver_proxy {
    virtual ~receiver_proxy() = default;

    virtual auto set_value() noexcept -> void                     = 0;
    virtual auto set_error(::std::exception_ptr) noexcept -> void = 0;
    virtual auto set_stopped() noexcept -> void                   = 0;

    template <class P, class Query>
        requires(::std::same_as<P, ::std::remove_cv_t<P>> && ::std::is_object_v<P> && !::std::is_array_v<P>)
    auto try_query(Query) noexcept -> ::std::optional<P> {
        // TODO(P2079R10): forward supported receiver environment queries
        // through this proxy, especially get_stop_token_t -> inplace_stop_token
        return ::std::nullopt;
    }
};

struct bulk_item_receiver_proxy : receiver_proxy {
    virtual auto execute(::std::size_t, ::std::size_t) noexcept -> void = 0;
};

struct parallel_scheduler_backend {
    virtual ~parallel_scheduler_backend() = default;

    virtual auto schedule(receiver_proxy&, ::std::span<::std::byte>) noexcept -> void = 0;
    virtual auto schedule_bulk_chunked(::std::size_t, bulk_item_receiver_proxy&, ::std::span<::std::byte>) noexcept
        -> void = 0;
    virtual auto schedule_bulk_unchunked(::std::size_t, bulk_item_receiver_proxy&, ::std::span<::std::byte>) noexcept
        -> void = 0;
};

// TODO(P2079R10): provide the project-supported link-time replaceability hook.
auto query_parallel_scheduler_backend() -> ::std::shared_ptr<parallel_scheduler_backend>;

} // namespace beman::execution::system_context_replaceability

namespace beman::execution {

class parallel_scheduler {
    using backend_type = ::beman::execution::system_context_replaceability::parallel_scheduler_backend;

  public:
    using scheduler_concept = ::beman::execution::scheduler_tag;

    class sender;

    parallel_scheduler()  = delete;
    ~parallel_scheduler() = default;

    parallel_scheduler(const parallel_scheduler&) noexcept                    = default;
    parallel_scheduler(parallel_scheduler&&) noexcept                         = default;
    auto operator=(const parallel_scheduler&) noexcept -> parallel_scheduler& = default;
    auto operator=(parallel_scheduler&&) noexcept -> parallel_scheduler&      = default;

    auto operator==(const parallel_scheduler& other) const noexcept -> bool {
        return this->backend_ == other.backend_;
    }

    static constexpr auto query(::beman::execution::get_forward_progress_guarantee_t) noexcept
        -> ::beman::execution::forward_progress_guarantee {
        return ::beman::execution::forward_progress_guarantee::parallel;
    }

    auto schedule() const noexcept -> sender;
    // TODO(P2079R10): customize bulk_chunked and bulk_unchunked for this scheduler.

  private:
    explicit parallel_scheduler(::std::shared_ptr<backend_type> backend) noexcept : backend_(::std::move(backend)) {}

    ::std::shared_ptr<backend_type> backend_;

    friend auto get_parallel_scheduler() -> parallel_scheduler;
};

class parallel_scheduler::sender {
    using backend_type = ::beman::execution::system_context_replaceability::parallel_scheduler_backend;

  public:
    using sender_concept = ::beman::execution::sender_tag;
    using completion_signatures =
        ::beman::execution::completion_signatures<::beman::execution::set_value_t(),
                                                  ::beman::execution::set_error_t(::std::exception_ptr),
                                                  ::beman::execution::set_stopped_t()>;

    class env {
      public:
        explicit env(::std::shared_ptr<backend_type> backend) noexcept : backend_(::std::move(backend)) {}

        auto
        query(const ::beman::execution::get_completion_scheduler_t<::beman::execution::set_value_t>&) const noexcept
            -> ::beman::execution::parallel_scheduler {
            return ::beman::execution::parallel_scheduler{this->backend_};
        }

      private:
        ::std::shared_ptr<backend_type> backend_;
    };

    template <::beman::execution::receiver Rcvr>
    class operation : public ::beman::execution::system_context_replaceability::receiver_proxy {
      public:
        using operation_state_concept = ::beman::execution::operation_state_tag;

        operation(::std::shared_ptr<backend_type> backend,
                  Rcvr&& rcvr) noexcept(::std::is_nothrow_constructible_v<::std::remove_cvref_t<Rcvr>, Rcvr>)
            : backend_(::std::move(backend)), rcvr_(::std::forward<Rcvr>(rcvr)) {}

        auto start() & noexcept -> void {
            // TODO(P2079R10): define backend storage sizing and stopped-before-start handling.
            this->backend_->schedule(*this, ::std::span<::std::byte>{this->storage_});
        }

      private:
        auto set_value() noexcept -> void override { ::beman::execution::set_value(::std::move(this->rcvr_)); }

        auto set_error(::std::exception_ptr error) noexcept -> void override {
            ::beman::execution::set_error(::std::move(this->rcvr_), ::std::move(error));
        }

        auto set_stopped() noexcept -> void override { ::beman::execution::set_stopped(::std::move(this->rcvr_)); }

        ::std::shared_ptr<backend_type> backend_;
        ::std::remove_cvref_t<Rcvr>     rcvr_;
        alignas(void*)::std::byte storage_[sizeof(void*) * 4]{};
    };

    explicit sender(::std::shared_ptr<backend_type> backend) noexcept : backend_(::std::move(backend)) {}

    template <typename...>
    static consteval auto get_completion_signatures() noexcept -> completion_signatures {
        return {};
    }

    auto get_env() const noexcept -> env { return env{this->backend_}; }

    template <::beman::execution::receiver Rcvr>
    auto connect(Rcvr&& rcvr) & noexcept(::std::is_nothrow_constructible_v<::std::remove_cvref_t<Rcvr>, Rcvr>)
        -> operation<Rcvr> {
        return operation<Rcvr>{this->backend_, ::std::forward<Rcvr>(rcvr)};
    }

    template <::beman::execution::receiver Rcvr>
    auto connect(Rcvr&& rcvr) && noexcept(::std::is_nothrow_constructible_v<::std::remove_cvref_t<Rcvr>, Rcvr>)
        -> operation<Rcvr> {
        return operation<Rcvr>{::std::move(this->backend_), ::std::forward<Rcvr>(rcvr)};
    }

  private:
    ::std::shared_ptr<backend_type> backend_;
};

inline auto parallel_scheduler::schedule() const noexcept -> sender { return sender{this->backend_}; }

// TODO(P2079R10): implement using system_context_replaceability::query_parallel_scheduler_backend().
auto get_parallel_scheduler() -> parallel_scheduler;

} // namespace beman::execution

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_PARALLEL_SCHEDULER
