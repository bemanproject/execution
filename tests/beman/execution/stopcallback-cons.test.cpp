// src/beman/execution/tests/stopcallback-cons.test.cpp
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/stop_token.hpp>
#include "test/execution.hpp"
#include <memory>
#include <type_traits>

TEST(stopcallback_cons) {
    // Reference: [stopcallback.cons] p1
    // Plan:
    // - Given a callback type Callback which can be constructed
    //   with an int*.
    // - When trying if a corresponding stop_callback<Callback> is
    //   constructible.
    // - Then the response is true when an int* is passed as
    //   initializer argument but false otherwise.
    struct Callback {
        explicit Callback(int*) {}
        auto operator()() -> void {}
    };
    static_assert(::std::is_constructible_v<::test_std::stop_callback<Callback>, const ::test_std::stop_token&, int*>);
    static_assert(
        not ::std::is_constructible_v<::test_std::stop_callback<Callback>, const ::test_std::stop_token&, bool*>);
    static_assert(::std::is_constructible_v<::test_std::stop_callback<Callback>, ::test_std::stop_token&&, int*>);
    static_assert(not ::std::is_constructible_v<::test_std::stop_callback<Callback>, ::test_std::stop_token&&, bool*>);

    // Reference: [stopcallback.cons] p2, p3
    // Plan:
    // The behavior can't be observed directly. It seem the best which can be
    // done is to verify that construction and destruction works even if the
    // callback is the only owner of the state.
    // - Given a stop_callback constructed from an engaged stop_token.
    // - When the corresponding stop_source and stop_token are destroyed.
    // - Then the callback can still be destroyed without issues.

    auto                                source = ::std::make_unique<::test_std::stop_source>();
    auto                                token  = ::std::make_unique<::test_std::stop_token>(source->get_token());
    ::test_std::stop_callback<Callback> cb(*token, nullptr);
}
