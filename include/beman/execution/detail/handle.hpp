// include/beman/execution/detail/handle.hpp                               -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_HANDLE
#define INCLUDED_BEMAN_EXECUTION_DETAIL_HANDLE

#include <beman/execution/execution.hpp>
#include <cassert>
#include <coroutine>
#include <memory>
#include <utility>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
template <typename P>
class handle {
  private:
    struct deleter {
        auto operator()(P* p) noexcept -> void {
            if (p) {
                std::coroutine_handle<P>::from_promise(*p).destroy();
            }
        }
    };
    std::unique_ptr<P, deleter> h;

  public:
    explicit handle(P* p) : h(p) {}
    auto reset() -> void { this->h.reset(); }
    template <typename... A>
    auto start(A&&... a) noexcept -> auto {
        return this->h->start(::std::forward<A>(a)...);
    }
    auto release() -> ::std::coroutine_handle<P> {
        return ::std::coroutine_handle<P>::from_promise(*this->h.release());
    }
    P*   get() const noexcept { return this->h.get(); }
    auto get_env() const noexcept {
        assert(this->h.get());
        return ::beman::execution::get_env(*this->h);
    }
};

} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif
