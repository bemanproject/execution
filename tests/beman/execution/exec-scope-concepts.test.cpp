// tests/beman/execution/exec-scope-concepts.test.cpp                 -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/detail/async_scope_token.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/sender_in.hpp>
#include <beman/execution/detail/completion_signatures.hpp>
#include <beman/execution/detail/set_value.hpp>
#include <test/execution.hpp>
#include <concepts>
#include <utility>

// ----------------------------------------------------------------------------

namespace {
struct sender {
    using sender_concept = test_std::sender_t;
};
static_assert(test_std::sender<sender>);

struct copyable {};
static_assert(std::copyable<copyable>);
struct non_copyable {
    non_copyable()                    = default;
    non_copyable(const non_copyable&) = delete;
};
static_assert(not std::copyable<non_copyable>);

struct empty {};

template <test_std::sender S>
struct wrap {
    using sender_concept = test_std::sender_t;
    std::remove_cvref_t<S> sndr;
    template <typename E>
    auto get_completion_signatures(const E& e) const noexcept {
        return test_std::get_completion_signatures(sndr, e);
    }
};
static_assert(test_std::sender<wrap<sender>>);

template <test_std::sender>
struct bad {
    using sender_concept = test_std::sender_t;
};

template <typename Mem, typename Bool, bool Noexcept, template <test_std::sender> class Wrap>
struct token {
    Mem  mem{};
    auto try_associate() -> Bool { return {}; }
    auto disassociate() noexcept(Noexcept) -> void {}
    template <test_std::sender Sender>
    auto wrap(Sender&& sndr) -> Wrap<Sender> {
        return Wrap<Sender>(std::forward<Sender>(sndr));
    }
};
} // namespace

TEST(exec_scope_concepts) {
    static_assert(test_std::async_scope_token<token<copyable, bool, true, wrap>>);
    static_assert(not test_std::async_scope_token<token<non_copyable, bool, true, wrap>>);
    static_assert(not test_std::async_scope_token<token<copyable, int, true, wrap>>);
    static_assert(not test_std::async_scope_token<token<copyable, bool, false, wrap>>);
    static_assert(not test_std::async_scope_token<token<copyable, bool, true, bad>>);
    static_assert(not test_std::async_scope_token<empty>);
}
