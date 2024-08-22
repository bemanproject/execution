// src/Beman/Execution26/tests/exec-get-domain.pass.cpp               -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <Beman/Execution26/execution.hpp>
#include <test/execution.hpp>
#include <concepts>

// ----------------------------------------------------------------------------

namespace
{
    struct domain { int value{}; };

    struct no_get_domain {};

    template <bool Noexcept>
    struct non_const_get_domain
    {
        auto query(test_std::get_domain_t const&) noexcept(Noexcept) -> domain
        {
            return {};
        }
    };


    template <bool Noexcept, typename Result>
    struct has_get_domain
    {
        int value{};
        constexpr auto query(test_std::get_domain_t const&) const noexcept(Noexcept) -> Result
        {
            return {this->value};
        }
    };

    struct overloaded_get_domain
    {
        auto query(test_std::get_domain_t const&) const noexcept -> domain
        {
            return {};
        }
        auto query(test_std::get_domain_t const&) noexcept -> void = delete;
    };

    template <bool Expect, typename Result, typename Object>
    auto test_get_domain(Object&& object)
    {
        static_assert(Expect == requires{ test_std::get_domain(object); });
        if constexpr (requires{ test_std::get_domain(object); })
        {
            static_assert(std::same_as<Result, decltype(test_std::get_domain(object))>);
        }
    }
}

auto main() -> int
{
    static_assert(std::same_as<test_std::get_domain_t const,
                               decltype(test_std::get_domain)>);

    static_assert(test_std::forwarding_query(test_std::get_domain));

    test_get_domain<false, bool>(no_get_domain{});
    test_get_domain<false, bool>(non_const_get_domain<true>{});
    test_get_domain<false, bool>(non_const_get_domain<false>{});
    test_get_domain<true, domain>(has_get_domain<true, domain>{42});
    test_get_domain<false, bool>(has_get_domain<false, domain>{42});
    test_get_domain<true, domain>(overloaded_get_domain{});

    assert(42 == test_std::get_domain(has_get_domain<true, domain>{42}).value);
    static_assert(42 == test_std::get_domain(has_get_domain<true, domain>{42}).value);
}