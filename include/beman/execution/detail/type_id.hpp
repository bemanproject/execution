// include/beman/execution/detail/type_id.hpp                 -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_TYPE_ID
#define INCLUDED_BEMAN_EXECUTION_DETAIL_TYPE_ID

#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <string_view>
#include <type_traits>
#include <utility>
#endif

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
struct type_info {
    ::std::string_view name;
    friend auto        operator==(const type_info&, const type_info&) noexcept -> bool = default;
};

// ReSharper disable once CppTemplateParameterNeverUsed
template <typename T>
consteval auto type_id_impl() -> type_info {
    ::std::string_view func_sig;
#if defined(__clang__)
    func_sig                  = __PRETTY_FUNCTION__;
    ::std::string_view prefix = "[T = ";
    auto               start  = func_sig.find(prefix);
    if (start != ::std::string_view::npos) {
        start += prefix.size();
        auto end = func_sig.find(']', start);
        if (end != ::std::string_view::npos) {
            return {func_sig.substr(start, end - start)};
        }
    }
#elif defined(_MSC_VER)
    func_sig                  = __FUNCSIG__;
    ::std::string_view prefix = "type_id_impl<";
    ::std::string_view suffix = ">(void)";

    auto start = func_sig.find(prefix);
    if (start != ::std::string_view::npos) {
        start += prefix.size();
        auto end = func_sig.rfind(suffix);
        if (end != ::std::string_view::npos && end > start) {
            return {func_sig.substr(start, end - start)};
        }
    }
#elif defined(__GNUC__)
    func_sig                  = __PRETTY_FUNCTION__;
    ::std::string_view prefix = "T = ";
    auto               start  = func_sig.find(prefix);
    if (start != ::std::string_view::npos) {
        start += prefix.size();
        auto end = func_sig.find_first_of(";]", start);
        if (end != ::std::string_view::npos) {
            return {func_sig.substr(start, end - start)};
        }
    }
#endif
    return {func_sig};
}

template <typename T>
consteval auto type_id() noexcept -> type_info {
    return ::beman::execution::detail::type_id_impl<::std::remove_cvref_t<T>>();
}
} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_TYPE_ID
