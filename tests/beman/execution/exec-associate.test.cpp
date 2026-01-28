// tests/beman/execution/exec-associate.test.cpp                      -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifdef BEMAN_USE_MODULE
import beman.execution;
#else
#include <beman/execution/detail/associate.hpp>
#include <beman/execution/detail/connect.hpp>
#include <beman/execution/detail/get_completion_signatures.hpp>
#include <beman/execution/detail/scope_token.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/receiver.hpp>
#include <beman/execution/detail/sync_wait.hpp>
#endif
#include <test/execution.hpp>
#include <concepts>
#include <type_traits>
#include <utility>

// ----------------------------------------------------------------------------

namespace {
struct sender {
    struct state {
        using operation_state_concept = test_std::operation_state_t;
        auto start() & noexcept {}
    };
    using sender_concept = test_std::sender_t;
    auto connect(auto&&) { return state(); }
};
static_assert(test_std::sender<sender>);

template <test_std::sender Sender>
struct wrap_sender {
    using sender_concept = test_std::sender_t;
    std::remove_cvref_t<Sender> sender;
    wrap_sender(Sender&& s) : sender(std::forward<Sender>(s)) {}

    template <typename Env>
    auto get_completion_signatures(const Env& env) const noexcept {
        return test_std::get_completion_signatures(this->sender, env);
    }
    template <test_std::receiver Receiver>
    auto connect(Receiver&& receiver) && {
        return test_std::connect(std::move(this->sender), std::forward<Receiver>(receiver));
    }
    template <test_std::receiver Receiver>
    auto connect(Receiver&& receiver) const& {
        return test_std::connect(this->sender, std::forward<Receiver>(receiver));
    }
};
template <test_std::sender Sender>
wrap_sender(Sender&& sender) -> wrap_sender<Sender>;

static_assert(test_std::sender<wrap_sender<sender>>);

template <bool Noexcept>
struct token {
    bool         value{};
    std::size_t* count{};
    auto         try_associate() noexcept(Noexcept) -> bool {
        if (this->value && this->count) {
            ++*this->count;
        }
        return this->value;
    }
    auto disassociate() noexcept -> void {
        if (this->count) {
            --*this->count;
        }
    }
    template <test_std::sender Sender>
    auto wrap(Sender&& sender) noexcept {
        return wrap_sender(std::forward<Sender>(sender));
    }
};
static_assert(test_std::scope_token<token<true>>);
static_assert(test_std::scope_token<token<false>>);

struct dtor_sender {
    std::size_t* count{};
    using sender_concept = test_std::sender_t;

    explicit dtor_sender(std::size_t* c) : count(c) {}
    dtor_sender(dtor_sender&& other) : count(std::exchange(other.count, nullptr)) {}
    ~dtor_sender() { ASSERT(count == nullptr || *count == 1u); }
};
static_assert(test_std::sender<dtor_sender>);

auto test_associate_data() {
    using data_t = test_detail::associate_data<token<true>, sender>;
    static_assert(std::same_as<wrap_sender<sender>, data_t::wrap_sender>);

    {
        test_detail::associate_data data(token<true>{true}, sender{});
        static_assert(std::same_as<data_t, decltype(data)>);
        ASSERT(data.sender);
        test_detail::associate_data move(std::move(data));
        ASSERT(!data.sender.has_value());
        ASSERT(move.sender);
    }
    {
        test_detail::associate_data data(token<true>{false}, sender{});
        static_assert(std::same_as<data_t, decltype(data)>);
        ASSERT(!data.sender);
    }
    {
        std::size_t count{};
        ASSERT(count == 0u);
        {
            test_detail::associate_data data(token<true>{true, &count}, dtor_sender{&count});
            ASSERT(count == 1u);
        }
        ASSERT(count == 0u);
    }
    {
        std::size_t count{};
        ASSERT(count == 0u);
        {
            test_detail::associate_data data(token<true>{true, &count}, sender{});
            ASSERT(count == 1u);
            ASSERT(data.sender);
            auto p{data.release()};
            ASSERT(count == 1u);
            ASSERT(!data.sender);
        }
        ASSERT(count == 1u);
    }
}

struct receiver {
    using receiver_concept = test_std::receiver_t;

    auto set_value(auto&&...) && noexcept {}
    auto set_error(auto&&) && noexcept {}
    auto set_stopped() && noexcept {}
};
static_assert(test_std::receiver<receiver>);

template <test_std::sender Sender, test_std::scope_token Token>
auto test_associate(Sender&& sender, Token&& token) -> void {
    [[maybe_unused]] auto sndr = test_std::associate(std::forward<Sender>(sender), std::forward<Token>(token));
    static_assert(test_std::sender<decltype(sndr)>);
    test_std::sync_wait(std::move(sndr));
}

} // namespace

TEST(exec_associate) {
    static_assert(std::same_as<const test_std::associate_t, decltype(test_std::associate)>);

    test_associate_data();

    test_associate(sender{}, token<true>{});
}
