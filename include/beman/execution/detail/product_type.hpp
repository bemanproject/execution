// include/beman/execution/detail/product_type.hpp                  -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_PRODUCT_TYPE
#define INCLUDED_BEMAN_EXECUTION_DETAIL_PRODUCT_TYPE

#include <memory>
#include <cstddef>

#include <beman/execution/detail/suppress_push.hpp>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {

template <::std::size_t I, typename T>
struct product_type_element {
    T    value;
    auto operator==(const product_type_element&) const -> bool = default;
};

template <typename, typename...>
struct product_type_base;

template <::std::size_t... I, typename... T>
struct product_type_base<::std::index_sequence<I...>, T...>
    : ::beman::execution::detail::product_type_element<I, T>... {
    static constexpr ::std::size_t size() { return sizeof...(T); }
    static constexpr bool          is_product_type{true};

    template <::std::size_t J, typename S>
    static auto element_get(::beman::execution::detail::product_type_element<J, S>& self) noexcept -> S& {
        return self.value;
    }
    template <::std::size_t J, typename S>
    static auto element_get(::beman::execution::detail::product_type_element<J, S>&& self) noexcept -> S&& {
        return ::std::move(self.value);
    }
    template <::std::size_t J, typename S>
    static auto element_get(const ::beman::execution::detail::product_type_element<J, S>& self) noexcept -> const S& {
        return self.value;
    }

    template <::std::size_t J>
    auto get() & -> decltype(auto) {
        return this->element_get<J>(*this);
    }
    template <::std::size_t J>
    auto get() && -> decltype(auto) {
        return this->element_get<J>(::std::move(*this));
    }
    template <::std::size_t J>
    auto get() const& -> decltype(auto) {
        return this->element_get<J>(*this);
    }

    template <::std::size_t J, typename Allocator, typename Self>
    static auto make_element(Allocator&& alloc, Self&& self) -> decltype(auto) {
        using type = ::std::remove_cvref_t<decltype(product_type_base::element_get<J>(std::forward<Self>(self)))>;
        if constexpr (::std::uses_allocator_v<type, Allocator>)
            return ::std::make_obj_using_allocator<type>(alloc,
                                                         product_type_base::element_get<J>(std::forward<Self>(self)));
        else
            return product_type_base::element_get<J>(std::forward<Self>(self));
    }

    auto operator==(const product_type_base&) const -> bool = default;
};

template <typename T>
concept is_product_type_c = requires(const T& t) { T::is_product_type; };

template <typename... T>
struct product_type : ::beman::execution::detail::product_type_base<::std::index_sequence_for<T...>, T...> {
    template <typename Allocator, typename Product, std::size_t... I>
    static auto make_from(Allocator&& allocator, Product&& product, std::index_sequence<I...>) -> product_type {
        return {product_type::template make_element<I>(allocator, ::std::forward<Product>(product))...};
    }

    template <typename Allocator, typename Product>
    static auto make_from(Allocator&& allocator, Product&& product) -> product_type {
        return product_type::make_from(
            ::std::forward<Allocator>(allocator), ::std::forward<Product>(product), ::std::index_sequence_for<T...>{});
    }

    template <typename Fun, ::std::size_t... I>
    constexpr auto apply_elements(::std::index_sequence<I...>, Fun&& fun) const -> decltype(auto) {
        return ::std::forward<Fun>(fun)(this->template get<I>()...);
    }
    template <typename Fun>
    constexpr auto apply(Fun&& fun) const -> decltype(auto) {
        return apply_elements(::std::index_sequence_for<T...>{}, ::std::forward<Fun>(fun));
    }
    template <typename Fun, ::std::size_t... I>
    constexpr auto apply_elements(::std::index_sequence<I...>, Fun&& fun) -> decltype(auto) {
        //-dk:TODO provide rvalue, lvalue, const lvalue overloads?
        return ::std::forward<Fun>(fun)(std::move(this->template get<I>())...);
    }
    template <typename Fun>
    constexpr auto apply(Fun&& fun) -> decltype(auto) {
        return apply_elements(::std::index_sequence_for<T...>{}, ::std::forward<Fun>(fun));
    }
};
template <typename... T>
product_type(T&&...) -> product_type<::std::decay_t<T>...>;

template <typename T>
constexpr auto is_product_type(const T&) -> ::std::false_type {
    return {};
}
template <typename... T>
constexpr auto is_product_type(const ::beman::execution::detail::product_type<T...>&) -> ::std::true_type {
    return {};
}

template <::std::size_t Start, typename Fun, typename Tuple, ::std::size_t... I>
constexpr auto sub_apply_helper(Fun&& fun, Tuple&& tuple, ::std::index_sequence<I...>) -> decltype(auto) {
    // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved)
    return ::std::forward<Fun>(fun)(::std::forward<Tuple>(tuple).template get<I + Start>()...);
}

template <::std::size_t Start, typename Fun, typename Tuple>
constexpr auto sub_apply(Fun&& fun, Tuple&& tuple) -> decltype(auto) {
    constexpr ::std::size_t TSize{::std::tuple_size_v<::std::remove_cvref_t<Tuple>>};
    static_assert(Start <= TSize);
    return sub_apply_helper<Start>(
        ::std::forward<Fun>(fun), ::std::forward<Tuple>(tuple), ::std::make_index_sequence<TSize - Start>());
}

} // namespace beman::execution::detail

namespace std {
template <typename T>
    requires ::beman::execution::detail::is_product_type_c<T>
struct tuple_size<T> : ::std::integral_constant<std::size_t, T::size()> {};

template <::std::size_t I, typename T>
    requires ::beman::execution::detail::is_product_type_c<T>
struct tuple_element<I, T> {
    using type = ::std::decay_t<decltype(::std::declval<T>().template get<I>())>;
};
} // namespace std

// ----------------------------------------------------------------------------

#include <beman/execution/detail/suppress_pop.hpp>

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_PRODUCT_TYPE
