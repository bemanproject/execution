// include/beman/execution/detail/stop_source.hpp                   -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_STOP_SOURCE
#define INCLUDED_BEMAN_EXECUTION_DETAIL_STOP_SOURCE

#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <atomic>
#include <memory>
#include <mutex>
#include <thread>
#include <type_traits>
#include <utility>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail.nostopstate;
#else
#include <beman/execution/detail/nostopstate.hpp>
#endif

// ----------------------------------------------------------------------------

namespace beman::execution {
class stop_token;
class stop_source;
template <typename CallbackFun>
class stop_callback;
template <typename CallbackFun>
stop_callback(::beman::execution::stop_token, CallbackFun) -> stop_callback<CallbackFun>;
} // namespace beman::execution

namespace beman::execution::detail {
struct stop_state;
struct stop_callback_base;
} // namespace beman::execution::detail
// ----------------------------------------------------------------------------

struct beman::execution::detail::stop_state {
    ::std::atomic<bool>                           stop_requested{};
    ::std::atomic<::std::size_t>                  sources{};
    ::std::mutex                                  lock{};
    beman::execution::detail::stop_callback_base* callbacks{};
    ::std::atomic<bool>                           executing{};

    auto stop_possible() const -> bool { return this->sources != 0 || this->stop_requested; }
};

// ----------------------------------------------------------------------------

struct beman::execution::detail::stop_callback_base {
  private:
    using stop_state = ::beman::execution::detail::stop_state;
    ::std::shared_ptr<stop_state> state;

    virtual auto do_call() -> void = 0;

  protected:
    explicit stop_callback_base(const ::beman::execution::stop_token&);
    ~stop_callback_base();

  public:
    stop_callback_base(stop_callback_base&&)                         = delete;
    stop_callback_base(const stop_callback_base&)                    = delete;
    auto operator=(stop_callback_base&&) -> stop_callback_base&      = delete;
    auto operator=(const stop_callback_base&) -> stop_callback_base& = delete;
    auto call() -> void;
    auto setup() -> void;
    auto deregister() -> void;

    stop_callback_base* next{};
    ::std::thread::id   id{};
};

// ----------------------------------------------------------------------------

class beman::execution::stop_source {
  private:
    using stop_state = ::beman::execution::detail::stop_state;

    ::std::shared_ptr<stop_state> state{::std::make_shared<stop_state>()};

  public:
    using stop_token = ::beman::execution::stop_token;

    stop_source();
    explicit stop_source(::beman::execution::nostopstate_t) noexcept;
    stop_source(const stop_source&);
    stop_source(stop_source&&) = default;
    auto operator=(const stop_source&) -> stop_source&;
    auto operator=(stop_source&&) -> stop_source& = default;
    ~stop_source();

    auto swap(stop_source&) noexcept -> void;
    auto get_token() const -> stop_token;
    auto stop_requested() const noexcept -> bool;
    auto stop_possible() const noexcept -> bool;
    auto request_stop() noexcept -> bool;
};

// ----------------------------------------------------------------------------

class beman::execution::stop_token {
  private:
    friend ::beman::execution::stop_source;
    friend ::beman::execution::detail::stop_callback_base;
    ::std::shared_ptr<::beman::execution::detail::stop_state> state;

    explicit stop_token(::std::shared_ptr<::beman::execution::detail::stop_state>);

  public:
    template <typename Fun>
    using callback_type = ::beman::execution::stop_callback<Fun>;

    stop_token() = default;

    auto               swap(stop_token& other) noexcept -> void;
    [[nodiscard]] auto stop_requested() const noexcept -> bool;
    [[nodiscard]] auto stop_possible() const noexcept -> bool;

    [[nodiscard]] auto operator==(const stop_token&) const noexcept -> bool = default;
};

// ----------------------------------------------------------------------------

template <typename CallbackFun>
class beman::execution::stop_callback final : private CallbackFun, beman::execution::detail::stop_callback_base {
  private:
    static_assert(::std::invocable<CallbackFun>);
    static_assert(::std::destructible<CallbackFun>);

    using stop_token = ::beman::execution::stop_token;

    auto do_call() -> void override { (*this)(); }

  public:
    using callback_type = CallbackFun;

    template <typename Initializer>
    stop_callback(const stop_token& token,
                  Initializer&&     init) noexcept(::std::is_nothrow_constructible_v<CallbackFun, Initializer>)
        requires(::std::constructible_from<CallbackFun, Initializer>)
        : CallbackFun(::std::forward<Initializer>(init)), stop_callback_base(token) {
        this->setup();
    }
    template <typename Initializer>
    stop_callback(stop_token&&  token,
                  Initializer&& init) noexcept(::std::is_nothrow_constructible_v<CallbackFun, Initializer>)
        requires(::std::is_constructible_v<CallbackFun, Initializer>)
        : CallbackFun(::std::forward<Initializer>(init)), stop_callback_base(::std::move(token)) {
        this->setup();
    }
    stop_callback(const stop_callback&) = delete;
    stop_callback(stop_callback&&)      = delete;
    ~stop_callback() { this->deregister(); }
    auto operator=(stop_callback&&) -> stop_callback&      = delete;
    auto operator=(const stop_callback&) -> stop_callback& = delete;
};

// ----------------------------------------------------------------------------

inline beman::execution::detail::stop_callback_base::stop_callback_base(const ::beman::execution::stop_token& token)
    : state(token.state) {}

inline beman::execution::detail::stop_callback_base::~stop_callback_base() {}

inline auto beman::execution::detail::stop_callback_base::setup() -> void {
    if (this->state) {
        {
            ::std::lock_guard guard(this->state->lock);
            if (!this->state->stop_requested) {
                this->next = ::std::exchange(this->state->callbacks, this);
                return;
            }
        }
        this->call();
    }
}

inline auto beman::execution::detail::stop_callback_base::deregister() -> void {
    if (this->state) {
        ::std::unique_lock guard(this->state->lock);
        if (this->state->executing && this->id != ::std::this_thread::get_id()) {
            using lock_again = decltype([](auto p) { p->lock(); });
            ::std::unique_ptr<decltype(guard), lock_again> relock(&guard);
            relock->unlock();
            while (this->state->executing)
                ;
        }
        for (auto n = &this->state->callbacks; *n; n = &this->next) {
            if (*n == this) {
                *n = this->next;
                break;
            }
        }
    }
}

inline auto beman::execution::detail::stop_callback_base::call() -> void { this->do_call(); }

inline beman::execution::stop_token::stop_token(::std::shared_ptr<::beman::execution::detail::stop_state> st)
    : state(::std::move(st)) {}

inline auto beman::execution::stop_token::swap(stop_token& other) noexcept -> void { this->state.swap(other.state); }

inline auto beman::execution::stop_token::stop_requested() const noexcept -> bool {
    return this->state && this->state->stop_requested;
}

inline auto beman::execution::stop_token::stop_possible() const noexcept -> bool {
    return this->state && this->state->stop_possible();
}

// ----------------------------------------------------------------------------

inline beman::execution::stop_source::stop_source() { ++this->state->sources; }

inline beman::execution::stop_source::stop_source(::beman::execution::nostopstate_t) noexcept : state() {}

inline beman::execution::stop_source::stop_source(const stop_source& other) : state(other.state) {
    ++this->state->sources;
}

inline auto beman::execution::stop_source::operator=(const stop_source& other) -> stop_source& {
    stop_source(other).swap(*this);
    return *this;
}

inline beman::execution::stop_source::~stop_source() { this->state && --this->state->sources; }

inline auto beman::execution::stop_source::swap(::beman::execution::stop_source& other) noexcept -> void {
    this->state.swap(other.state);
}

inline auto beman::execution::stop_source::get_token() const -> stop_token { return stop_token{this->state}; }

inline auto beman::execution::stop_source::stop_requested() const noexcept -> bool {
    return this->state && this->state->stop_requested;
}

inline auto beman::execution::stop_source::stop_possible() const noexcept -> bool { return true && this->state; }

inline auto beman::execution::stop_source::request_stop() noexcept -> bool {
    using release    = decltype([](auto p) { *p = false; });
    using lock_again = decltype([](auto p) { p->lock(); });

    if (this->state && not this->state->stop_requested.exchange(true)) {
        ::std::unique_lock guard(this->state->lock);
        while (this->state->callbacks) {
            auto front = ::std::exchange(this->state->callbacks, this->state->callbacks->next);
            ::std::unique_ptr<::std::atomic<bool>, release> reset(&state->executing);
            *reset    = true;
            front->id = ::std::this_thread::get_id();
            ::std::unique_ptr<decltype(guard), lock_again> relock(&guard);
            relock->unlock();
            front->call();
        }
        return true;
    }
    return false;
}

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_STOP_SOURCE
