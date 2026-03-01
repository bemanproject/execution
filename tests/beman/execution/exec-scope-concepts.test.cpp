// tests/beman/execution/exec-scope-concepts.test.cpp                 -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <concepts>
#include <utility>
#include <test/execution.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution/detail/scope_token.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/sender_in.hpp>
#include <beman/execution/detail/completion_signatures.hpp>
#include <beman/execution/detail/set_value.hpp>
#endif

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

template <typename S>
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

template <bool Noexcept>
struct association {
    association() = default;

    association(const association&) = delete;

    association(association&&) noexcept(Noexcept) = default;

    auto operator=(const association&) -> association& = delete;

    auto operator=(association&&) noexcept(Noexcept) -> association& = default;

    static auto try_associate() noexcept -> association { return {}; }

    explicit operator bool() const noexcept { return false; }
};

template <typename Mem, typename Assoc, template <test_std::sender> class Wrap>
struct token {
    Mem  mem{};
    auto try_associate() const -> Assoc { return {}; }

    template <test_std::sender Sender>
    auto wrap(Sender&& sndr) const -> Wrap<Sender> {
        return Wrap<Sender>(std::forward<Sender>(sndr));
    }
};
} // namespace

TEST(exec_scope_concepts) {

    static_assert(test_std::scope_token<token<copyable, association<true>, wrap>>);
    static_assert(not test_std::scope_token<token<non_copyable, association<true>, wrap>>);
    static_assert(not test_std::scope_token<token<copyable, association<false>, wrap>>);
    static_assert(not test_std::scope_token<token<copyable, association<true>, bad>>);
    static_assert(not test_std::scope_token<empty>);
}
