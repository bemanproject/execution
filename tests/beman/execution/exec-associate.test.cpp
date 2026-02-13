// tests/beman/execution/exec-associate.test.cpp                          -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <test/execution.hpp>
#include <concepts>
#include <memory>
#include <optional>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <beman/execution/detail/associate.hpp>
#include <beman/execution/detail/completion_signatures_of_t.hpp>
#include <beman/execution/detail/just.hpp>
#include <beman/execution/detail/set_error.hpp>
#include <beman/execution/detail/set_stopped.hpp>
#include <beman/execution/detail/set_value.hpp>
#include <beman/execution/detail/sync_wait.hpp>
#include <beman/execution/detail/then.hpp>

namespace {
// A scope token that always associates successfully and doesn't wrap the sender.
struct null_token {
    struct assoc {
        constexpr explicit operator bool() const noexcept { return true; }
        constexpr assoc    try_associate() const noexcept { return {}; }
    };

    template <test_std::sender Sender>
    constexpr Sender&& wrap(Sender&& sndr) const noexcept {
        return std::forward<Sender>(sndr);
    }

    constexpr assoc try_associate() const noexcept { return {}; }
};
static_assert(test_std::scope_token<null_token>);

// A scope token that always fails association.
struct expired_token {
    struct assoc {
        constexpr explicit operator bool() const noexcept { return false; }
        constexpr assoc    try_associate() const noexcept { return {}; }
    };

    template <test_std::sender Sender>
    constexpr Sender&& wrap(Sender&& sndr) const noexcept {
        return std::forward<Sender>(sndr);
    }

    constexpr assoc try_associate() const noexcept { return {}; }
};
static_assert(test_std::scope_token<expired_token>);

// Helper: does the sender complete with a value using sync_wait().
template <class Sender>
auto completes_with_value(Sender&& snd) -> bool {
    // If it completes with stopped or error, sync_wait returns empty optional.
    return static_cast<bool>(test_std::sync_wait(std::forward<Sender>(snd)));
}

struct scope {
    bool open{true};

    struct assoc {
        constexpr explicit operator bool() const noexcept { return !!scope_; }

        constexpr assoc try_associate() const noexcept { return assoc{scope_ && scope_->open ? scope_ : nullptr}; }

        const scope* scope_{};
    };

    struct token {
        template <test_std::sender Sender>
        constexpr Sender&& wrap(Sender&& sndr) const noexcept {
            return std::forward<Sender>(sndr);
        }

        constexpr assoc try_associate() const noexcept { return assoc{scope_ && scope_->open ? scope_ : nullptr}; }

        const scope* scope_{};
    };

    constexpr token get_token() const noexcept { return token{this}; }
};
static_assert(test_std::scope_token<scope::token>);
static_assert(test_std::scope_association<scope::assoc>);

} // namespace

TEST(exec_associate) {
    // associate returns a sender
    {
        using snd_t = decltype(test_std::associate(test_std::just(), null_token{}));
        static_assert(test_std::sender<snd_t>);
        static_assert(test_std::sender<snd_t&>);
        static_assert(test_std::sender<const snd_t&>);
    }

    // completion signatures: this implementation currently reports set_value() only.
    {
        using snd0_t = decltype(test_std::associate(test_std::just(), null_token{}));
        static_assert(std::same_as<test_std::completion_signatures<test_std::set_value_t()>,
                                   test_std::completion_signatures_of_t<snd0_t, test_std::env<>>>);

        using snd1_t = decltype(test_std::associate(test_std::just(std::string{}), null_token{}));
        static_assert(std::same_as<test_std::completion_signatures<test_std::set_value_t(std::string)>,
                                   test_std::completion_signatures_of_t<snd1_t, test_std::env<>>>);

        using snd2_t = decltype(test_std::associate(test_std::just_stopped(), null_token{}));
        static_assert(std::same_as<test_std::completion_signatures<test_std::set_stopped_t()>,
                                   test_std::completion_signatures_of_t<snd2_t, test_std::env<>>>);

        using snd3_t = decltype(test_std::associate(test_std::just_error(5), null_token{}));
        static_assert(std::same_as<test_std::completion_signatures<test_std::set_error_t(int)>,
                                   test_std::completion_signatures_of_t<snd3_t, test_std::env<>>>);

        int i        = 42;
        using snd4_t = decltype(test_std::associate(test_std::just(std::ref(i)), null_token{}));
        static_assert(std::same_as<test_std::completion_signatures<test_std::set_value_t(std::reference_wrapper<int>)>,
                                   test_std::completion_signatures_of_t<snd4_t, test_std::env<>>>);
    }

    // Identity behavior with null_token for value path + piping works.
    {
        {
            auto r = test_std::sync_wait(test_std::just() | test_std::associate(null_token{}));
            ASSERT(r.has_value());
        }
        {
            auto r = test_std::sync_wait(test_std::just(42) | test_std::associate(null_token{}));
            ASSERT(r.has_value());
            ASSERT((std::get<0>(r.value()) == 42));
        }
        {
            auto r = test_std::sync_wait(test_std::just(42, 67) | test_std::associate(null_token{}));
            ASSERT(r.has_value());
            ASSERT((std::get<0>(r.value()) == 42));
            ASSERT((std::get<1>(r.value()) == 67));
        }

        int i = 42;
        {
            auto r = test_std::sync_wait(test_std::just(std::ref(i)) | test_std::associate(null_token{}));
            ASSERT(r.has_value());
            ASSERT((&std::get<0>(r.value()).get() == &i));
        }

        {
            auto r = test_std::sync_wait(test_std::just(42) | test_std::associate(null_token{}) |
                                         test_std::then([](int x) noexcept { return x; }));
            ASSERT(r.has_value());
            ASSERT((std::get<0>(r.value()) == 42));
        }

        // On error/stopped paths, null_token shouldn't interfere: upon_* should still run.
        {
            auto r = test_std::sync_wait(test_std::just_error(42) | test_std::associate(null_token{}) |
                                         test_std::upon_error([](int i) { return i; }));
            ASSERT(r.has_value());
            ASSERT((std::get<0>(r.value()) == 42));
        }
        {
            auto r = test_std::sync_wait(test_std::just_stopped() | test_std::associate(null_token{}) |
                                         test_std::upon_stopped([]() noexcept { return 42; }));
            ASSERT(r.has_value());
            ASSERT((std::get<0>(r.value()) == 42));
        }
    }

    // associate is just_stopped with expired_token
    {
        auto snd = test_std::just(true) | test_std::associate(expired_token{}) |
                   test_std::upon_stopped([]() noexcept { return false; });
        auto r = test_std::sync_wait(std::move(snd));
        ASSERT(r.has_value());
        ASSERT((std::get<0>(r.value()) == false));
    }

    // Copying an associate-sender re-queries for a new association
    {
        scope s;
        auto  snd = test_std::associate(test_std::just(42), s.get_token());
        // while open, copy should succeed
        {
            auto r = test_std::sync_wait(snd | test_std::upon_stopped([]() noexcept { return 67; }));
            ASSERT(r.has_value());
            ASSERT((std::get<0>(r.value()) == 42));
        }

        s.open = false;

        // after closing, a fresh copy should stop
        {
            auto r = test_std::sync_wait(snd | test_std::upon_stopped([]() noexcept { return 67; }));
            ASSERT(r.has_value());
            ASSERT((std::get<0>(r.value()) == 67));
        }

        // original should still succeed even though started after closing
        {
            auto r = test_std::sync_wait(std::move(snd));
            ASSERT(r.has_value());
            ASSERT((std::get<0>(r.value()) == 42));
        }
    }

    // The sender argument is eagerly destroyed when try_associate fails.
    {
        bool deleted    = false;
        using deleter_t = decltype([](bool* p) noexcept { *p = true; });
        std::unique_ptr<bool, deleter_t> ptr(&deleted);

        auto snd = test_std::just(std::move(ptr)) | test_std::associate(expired_token{});

        ASSERT(deleted == true);
        (void)snd;
    }

    // A quick runtime sanity check for the stopped vs value branch.
    {
        ASSERT(completes_with_value(test_std::just(1) | test_std::associate(null_token{})));
        ASSERT(!completes_with_value(test_std::just(1) | test_std::associate(expired_token{})));
    }
}
