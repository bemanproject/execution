// src/beman/execution/tests/meta-filter.test.cpp                   -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <concepts>
#include <test/execution.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail;
#else
#include <beman/execution/detail/meta_filter.hpp>
#endif

// ----------------------------------------------------------------------------

namespace {
struct arg {};
template <typename...>
struct type_list {};

template <typename T>
struct is_char {
    static constexpr bool value = std::same_as<char, T>;
};
} // namespace

TEST(meta_filter) {
    static_assert(std::same_as<type_list<>, test_detail::meta::filter<is_char, type_list<>>>);
    static_assert(std::same_as<type_list<>, test_detail::meta::filter<is_char, type_list<arg, bool, double>>>);
    static_assert(
        std::same_as<type_list<char, char, char, char>,
                     test_detail::meta::filter<is_char, type_list<char, arg, char, bool, char, double, char>>>);
}
