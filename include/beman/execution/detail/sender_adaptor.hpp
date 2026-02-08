// include/beman/execution/detail/sender_adaptor.hpp                -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_SENDER_ADAPTOR
#define INCLUDED_BEMAN_EXECUTION_DETAIL_SENDER_ADAPTOR

#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <type_traits>
#include <utility>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail.forward_like;
import beman.execution.detail.product_type;
import beman.execution.detail.sender;
import beman.execution.detail.sender_adaptor_closure;
import beman.execution.detail.sender_decompose;
#else
#include <beman/execution/detail/forward_like.hpp>
#include <beman/execution/detail/product_type.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/sender_adaptor_closure.hpp>
#include <beman/execution/detail/sender_decompose.hpp>
#endif

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
template <typename Adaptor, typename... T> //-dk:TODO detail export
struct sender_adaptor : ::beman::execution::detail::product_type<::std::decay_t<Adaptor>, ::std::decay_t<T>...>,
                        ::beman::execution::sender_adaptor_closure<sender_adaptor<Adaptor, T...>> {
    template <::beman::execution::sender Sender, typename Self>
    static auto apply(Sender&& sender, Self&& self) {
        return [&self, &sender]<::std::size_t... I>(::std::index_sequence<I...>) {
            auto&& fun(self.template get<0>());
            return fun(::std::forward<Sender>(sender),
                       ::beman::execution::detail::forward_like<Self>(self.template get<I + 1>())...);
        }(::std::make_index_sequence<sender_adaptor::size() - 1u>{});
    }
    template <::beman::execution::sender Sender>
    auto operator()(Sender&& sender) {
        return apply(::std::forward<Sender>(sender), ::std::move(*this));
    }
    template <::beman::execution::sender Sender>
    auto operator()(Sender&& sender) const {
        return apply(::std::forward<Sender>(sender), *this);
    }
};
template <typename... T>
sender_adaptor(T&&...) -> sender_adaptor<T...>;
} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_SENDER_ADAPTOR
