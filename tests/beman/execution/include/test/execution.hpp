// include/test/execution.hpp -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_TEST_EXECUTION
#define INCLUDED_TEST_EXECUTION

#include <beman/execution/stop_token.hpp>
#include <concepts>
#include <cstddef>
#ifndef _MSC_VER
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#endif
#include <source_location>

#undef NDEBUG
#include <cassert>

#define ASSERT(condition) assert(condition)
#define ASSERT_UNREACHABLE() assert(::test::unreachable_helper())
#define TEST(name) auto main() -> int

namespace beman::execution {}

namespace test_std    = ::beman::execution;
namespace test_detail = ::beman::execution::detail;

namespace test {
#if 201907L <= __cpp_lib_source_location
using source_location = ::std::source_location;
#else
struct source_location {
    static auto current() -> source_location { return {}; }
    auto        file_name() const -> const char* { return "<unknown:no std::source_location>"; }
    auto        line() const -> const char* { return "<unknown:no std::source_location>"; }
};
#endif

inline bool unreachable_helper() { return false; }

template <typename>
auto type_exists() {}
template <typename T0, typename T1>
auto check_type(T1&&) {
    static_assert(std::same_as<T0, T1>);
}

auto use(auto&&...) noexcept -> void {}

struct throws {
    throws()                                                 = default;
    throws(throws&&) noexcept(false)                         = default;
    throws(const throws&) noexcept(false)                    = default;
    ~throws()                                                = default;
    auto operator=(throws&&) noexcept(false) -> throws&      = default;
    auto operator=(const throws&) noexcept(false) -> throws& = default;
};

inline auto death([[maybe_unused]] auto                   fun,
                  [[maybe_unused]] ::std::source_location location = test::source_location::current()) noexcept
    -> void {
#ifndef _MSC_VER
    switch (::pid_t rc = ::fork()) {
    default: {
        int stat{};
        ASSERT(rc == ::wait(&stat));
        if (stat == EXIT_SUCCESS) {
            ::std::cerr << "failed death test at " << "file=" << location.file_name() << ":" << location.line() << "\n"
                        << std::flush;
            ASSERT(stat != EXIT_SUCCESS);
        }
    } break;
    case 0: {
        ::close(2);
        fun();
        ::std::exit(EXIT_SUCCESS);
    } break;
    }
#endif
}
} // namespace test

#endif // INCLUDED_TEST_EXECUTION
