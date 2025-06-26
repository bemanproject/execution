// include/beman/execution/detail/counting_scope.hpp                  -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_COUNTING_SCOPE
#define INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_COUNTING_SCOPE

#include <beman/execution/detail/counting_scope_base.hpp>
#include <beman/execution/detail/counting_scope_join.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/inplace_stop_source.hpp>
#include <beman/execution/detail/stop_when.hpp>
#include <utility>
#include <cstdlib>

// ----------------------------------------------------------------------------

namespace beman::execution {
class counting_scope;
}

// ----------------------------------------------------------------------------

class beman::execution::counting_scope : public ::beman::execution::detail::counting_scope_base {
  public:
    class token;

    auto join() noexcept -> ::beman::execution::sender auto {
        return ::beman::execution::detail::counting_scope_join(this);
    }
    auto get_token() noexcept -> token;
    auto request_stop() noexcept -> void { this->stop_source.request_stop(); }

  private:
    ::beman::execution::inplace_stop_source stop_source{};
};

// ----------------------------------------------------------------------------

class beman::execution::counting_scope::token : public ::beman::execution::detail::counting_scope_base::token {
  public:
    template <::beman::execution::sender Sender>
    auto wrap(Sender&& sender) const noexcept -> ::beman::execution::sender auto {
        return ::beman::execution::detail::stop_when(
            ::std::forward<Sender>(sender),
            static_cast<::beman::execution::counting_scope*>(this->scope)->stop_source.get_token());
    }

  private:
    friend class beman::execution::counting_scope;
    explicit token(::beman::execution::counting_scope* s)
        : ::beman::execution::detail::counting_scope_base::token(s) {}
};

// ----------------------------------------------------------------------------

inline auto beman::execution::counting_scope::get_token() noexcept -> beman::execution::counting_scope::token {
    return beman::execution::counting_scope::token(this);
}

#endif
