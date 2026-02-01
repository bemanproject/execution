// src/beman/execution/tests/stopsource-cons.test.cpp
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <test/execution.hpp>
#include <new>
#include <stdexcept>
#include <cassert>
#include <cstddef>
#include <cstring>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution/stop_token.hpp>
#endif

namespace {
bool fail{};

}

// NOLINTNEXTLINE(hicpp-no-malloc)
auto operator new(::std::size_t size) -> void* { return fail ? throw ::std::bad_alloc() : ::std::malloc(size); }

auto operator delete(void* ptr) noexcept -> void { ::std::free(ptr); }                // NOLINT(hicpp-no-malloc)
auto operator delete(void* ptr, ::std::size_t) noexcept -> void { ::std::free(ptr); } // NOLINT(hicpp-no-malloc)

TEST(stopsource_cons) {
    // Reference: [stopsource.cons] p1
    try {
        ::test_std::stop_source source;
        ASSERT(source.stop_possible());
        ASSERT((not source.stop_requested()));
    } catch (...) {
        ASSERT(nullptr == "can't be reached"); // NOLINT(cert-dcl03-c,hicpp-static-assert,misc-static-assert)
    }

    // Reference: [stopsource.cons] p3
    fail = true;
    try {
        ::test_std::stop_source source{};
        test::use(source);
        ASSERT(nullptr == "can't be reached"); // NOLINT(cert-dcl03-c,hicpp-static-assert,misc-static-assert)
    } catch (const ::std::bad_alloc&) {
        ASSERT(nullptr != "bad_alloc was thrown"); // NOLINT(cert-dcl03-c,hicpp-static-assert,misc-static-assert)
    } catch (...) {
        ASSERT(nullptr == "can't be reached"); // NOLINT(cert-dcl03-c,hicpp-static-assert,misc-static-assert)
    }
}
