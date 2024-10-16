// include/beman/execution26/detail/product_type.hpp                  -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_PRODUCT_TYPE
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_PRODUCT_TYPE

#include <memory>
#include <cstddef>

// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    template <::std::size_t, typename T>
    struct product_type_element
    {
        T value;
        auto operator== (product_type_element const&) const -> bool = default;
    };

    template <typename, typename...>
    struct product_type_base;

    template <::std::size_t... I, typename... T>
    struct product_type_base<::std::index_sequence<I...>, T...>
        : ::beman::execution26::detail::product_type_element<I, T>...
    {
        static constexpr ::std::size_t size() { return sizeof...(T); }

        template <::std::size_t J, typename S>
        static auto element_get(
            ::beman::execution26::detail::product_type_element<J, S>& self)
            noexcept
            -> S&
        {
            return self.value;
        }
        template <::std::size_t J, typename S>
        static auto element_get(
            ::beman::execution26::detail::product_type_element<J, S>&& self)
            noexcept
            -> S
        {
            return ::std::move(self.value);
        }
        template <::std::size_t J, typename S>
        static auto element_get(
            ::beman::execution26::detail::product_type_element<J, S> const& self)
            noexcept
            -> S const&
        {
            return self.value;
        }

        template <::std::size_t J>
        auto get() & -> decltype(auto)
        {
            return this->element_get<J>(*this);
        }
        template <::std::size_t J>
        auto get() && -> decltype(auto)
        {
            return this->element_get<J>(::std::move(*this));
        }
        template <::std::size_t J>
        auto get() const& -> decltype(auto)
        {
            return this->element_get<J>(::std::move(*this));
        }

        template <::std::size_t J, typename Allocator, typename Self>
        static auto make_element(Allocator&& alloc, Self&& self) -> decltype(auto)
        {
            using type = ::std::remove_cvref_t<decltype(std::forward<Self>(self).template element_get<J>(std::forward<Self>(self)))>;
            if constexpr (::std::uses_allocator_v<type, Allocator>)
                return ::std::make_obj_using_allocator<type>(alloc, std::forward<Self>(self).template element_get<J>(std::forward<Self>(self)));
            else
                return std::forward<Self>(self).template element_get<J>(std::forward<Self>(self));
        }

        auto operator== (product_type_base const&) const -> bool = default;
    };

    template <typename... T>
    struct product_type
        : ::beman::execution26::detail::product_type_base<::std::index_sequence_for<T...>, T...>
    {
        template <typename Allocator, typename Product, std::size_t... I>
        static auto make_from(Allocator&& allocator, Product&& product, std::index_sequence<I...>)
            -> product_type
        {
            return { ::std::forward<Product>(product).template make_element<I>(allocator, ::std::forward<Product>(product))... };
        }

        template <typename Allocator, typename Product>
        static auto make_from(Allocator&& allocator, Product&& product) -> product_type
        {
            return std::forward<Product>(product).make_from(
                ::std::forward<Allocator>(allocator),
                ::std::forward<Product>(product),
                ::std::index_sequence_for<T...>{}
            );
        }

        template <typename Fun, ::std::size_t... I>
        constexpr auto apply_elements(::std::index_sequence<I...>, Fun&& fun) const -> decltype(auto)
        {
            return ::std::forward<Fun>(fun)(this->template get<I>()...);
        }
        template <typename Fun>
        constexpr auto apply(Fun&& fun) const -> decltype(auto)
        {
            return apply_elements(::std::index_sequence_for<T...>{}, ::std::forward<Fun>(fun));
        }
        template <typename Fun, ::std::size_t... I>
        constexpr auto apply_elements(::std::index_sequence<I...>, Fun&& fun) -> decltype(auto)
        {
            return ::std::forward<Fun>(fun)(this->template get<I>()...);
        }
        template <typename Fun>
        constexpr auto apply(Fun&& fun) -> decltype(auto)
        {
            return apply_elements(::std::index_sequence_for<T...>{}, ::std::forward<Fun>(fun));
        }
    };
    template <typename... T>
    product_type(T&&...) -> product_type<::std::decay_t<T>...>;

    template <typename T>
    constexpr auto is_product_type(T const&) -> ::std::false_type { return {}; }
    template <typename... T>
    constexpr auto is_product_type(::beman::execution26::detail::product_type<T...> const&) -> ::std::true_type { return {}; }
}

namespace std
{
    template <typename...T>
    struct tuple_size<::beman::execution26::detail::product_type<T...>>
        : ::std::integral_constant<std::size_t, sizeof...(T)>
    {
    };
    template <::std::size_t I, typename...T>
    struct tuple_element<I, ::beman::execution26::detail::product_type<T...>>
    {
        using type = ::std::decay_t<decltype(::std::declval<::beman::execution26::detail::product_type<T...>>().template get<I>())>;
    };
}

// ----------------------------------------------------------------------------

#endif
