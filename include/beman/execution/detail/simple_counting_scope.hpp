// include/beman/execution/detail/simple_counting_scope.hpp         -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_SIMPLE_COUNTING_SCOPE
#define INCLUDED_BEMAN_EXECUTION_DETAIL_SIMPLE_COUNTING_SCOPE

#include <beman/execution/detail/common.hpp>
#include <beman/execution/detail/counting_scope_base.hpp>
#include <beman/execution/detail/counting_scope_join.hpp>
#include <beman/execution/detail/sender.hpp>
#include <utility>
#include <cstdlib>

// ----------------------------------------------------------------------------

namespace beman::execution {
class simple_counting_scope;
}

// ----------------------------------------------------------------------------

class beman::execution::simple_counting_scope : public ::beman::execution::detail::counting_scope_base {
  public:
    class token;

    auto get_token() noexcept -> token;
    auto join() noexcept -> ::beman::execution::sender auto {
        return ::beman::execution::detail::counting_scope_join(this);
    }
};

// ----------------------------------------------------------------------------

class beman::execution::simple_counting_scope::token : public ::beman::execution::detail::counting_scope_base::token {
  public:
    template <::beman::execution::sender Sender>
    auto wrap(Sender&& sender) const noexcept -> Sender&& {
        return ::std::forward<Sender>(sender);
    }

  private:
    friend class beman::execution::simple_counting_scope;
    explicit token(::beman::execution::detail::counting_scope_base* s)
        : ::beman::execution::detail::counting_scope_base::token(s) {}
};

// ----------------------------------------------------------------------------

inline auto beman::execution::simple_counting_scope::get_token() noexcept
    -> beman::execution::simple_counting_scope::token {
    return beman::execution::simple_counting_scope::token(this);
}

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_SIMPLE_COUNTING_SCOPE
