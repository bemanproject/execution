// src/beman/execution/tests/exec-within.test.cpp                     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <stdexcept>
#include <system_error>
#include <test/execution.hpp>
#include <test/completion_test.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
import beman.execution.detail.completion_storage;
#else
#include <beman/execution/detail/completion_storage.hpp>
#endif

// ----------------------------------------------------------------------------

/// Receiver that invokes a function when any of the receiver's completion signals are called.
template <typename Func>
struct func_receiver {
    using receiver_concept = test_std::receiver_tag;

    template <typename... Args>
    void set_value(Args&&... args) noexcept {
        std::move(func)(test_std::set_value_t{}, ::std::forward<Args>(args)...);
    }

    template <typename E>
    void set_error(E&& e) noexcept {
        std::move(func)(test_std::set_error_t{}, ::std::forward<E>(e));
    }

    void set_stopped() noexcept { std::move(func)(test_std::set_stopped_t{}); }

    Func func;
};

/// Checks that we can store a trivial `set_value` completion, with no values.
auto test_store_trivial_set_value() -> void {
    // Arrange
    using sigs_t    = test_std::completion_signatures<test_std::set_value_t()>;
    using storage_t = test_detail::completion_storage<sigs_t>;
    storage_t sut;
    bool      executed{false};
    auto      recv = func_receiver{[&](auto tag, auto&&... args) noexcept {
        ASSERT((std::same_as<decltype(tag), test_std::set_value_t>));
        ASSERT(sizeof...(args) == 0);
        executed = true;
    }};

    // Act
    sut.store(test_std::set_value_t{});
    std::move(sut).complete(std::move(recv));

    // Assert
    ASSERT(executed);
}

/// Checks that we can store a trivial `set_error` completion, with a simple integer error code.
auto test_store_trivial_set_error() -> void {
    // Arrange
    using sigs_t    = test_std::completion_signatures<test_std::set_error_t(int)>;
    using storage_t = test_detail::completion_storage<sigs_t>;
    storage_t sut;
    bool      executed{false};
    auto      recv = func_receiver{[&](auto tag, int ec) noexcept {
        ASSERT((std::same_as<decltype(tag), test_std::set_error_t>));
        ASSERT(ec == -1);
        executed = true;
    }};

    // Act
    sut.store(test_std::set_error_t{}, -1);
    std::move(sut).complete(std::move(recv));

    // Assert
    ASSERT(executed);
}

/// Checks that we can store a trivial `set_stopped` completion.
auto test_store_trivial_set_stopped() -> void {
    // Arrange
    using sigs_t    = test_std::completion_signatures<test_std::set_stopped_t()>;
    using storage_t = test_detail::completion_storage<sigs_t>;
    storage_t sut;
    bool      executed{false};
    auto      recv = func_receiver{[&](auto tag) noexcept {
        ASSERT((std::same_as<decltype(tag), test_std::set_stopped_t>));
        executed = true;
    }};

    // Act
    sut.store(test_std::set_stopped_t{});
    std::move(sut).complete(std::move(recv));

    // Assert
    ASSERT(executed);
}

/// Checks that we can store data for a complex completion signature.
auto test_store_with_complex_signatures() -> void {
    // Arrange
    using sigs_t    = test_std::completion_signatures<test_std::set_value_t(),
                                                      test_std::set_value_t(int, int, int),
                                                      test_std::set_value_t(double, double, double),
                                                      test_std::set_value_t(float, float, float),
                                                      test_std::set_value_t(short, short, short),
                                                      test_std::set_value_t(char*, char*, char*),
                                                      test_std::set_error_t(int),
                                                      test_std::set_error_t(std::error_code),
                                                      test_std::set_error_t(std::exception_ptr),
                                                      test_std::set_stopped_t()>;
    using storage_t = test_detail::completion_storage<sigs_t>;
    storage_t sut;
    bool      executed{false};
    auto      recv = func_receiver{[&](auto tag, auto... args) noexcept {
        ASSERT((std::same_as<decltype(tag), test_std::set_value_t>));
        if constexpr (std::is_same_v<std::tuple<decltype(args)...>, std::tuple<double, double, double>>) {
            auto [x, y, z] = std::forward_as_tuple(args...);
            ASSERT(x == 3.14);
            ASSERT(y == 2.71);
            ASSERT(z == 1.61);
        } else {
            ASSERT(false);
        }
        executed = true;
    }};

    // Act
    sut.store(test_std::set_value_t{}, 3.14, 2.71, 1.61);
    std::move(sut).complete(std::move(recv));

    // Assert
    ASSERT(executed);
    auto max_size = std::max(sizeof(double), sizeof(char*));
    max_size      = std::max(max_size, sizeof(std::error_code));
    max_size      = std::max(max_size, sizeof(std::exception_ptr));
    ASSERT(sizeof(sut) <= 3 * max_size + 2 * sizeof(void*)); // should be able to store the largest completion
                                                             // signature, plus some overhead for the variant.
}

/// Type that throws on move after a certain number of moves.
struct throw_on_move {
    int x;
    int iteration{1};

    throw_on_move(int xx, int i = 1) : x(xx), iteration(i) {}
    throw_on_move(throw_on_move&& other) : x(other.x), iteration(other.iteration) {
        if (--iteration <= 0) {
            throw std::runtime_error("throw_on_move");
        }
    }
    throw_on_move(const throw_on_move&) = delete;
};

/// Checks that when we throw when storing the completion, we store a `set_error` completion with the current
/// exception.
auto test_throw_on_move_when_storing() -> void {
    // Arrange
    using sigs_t    = test_std::completion_signatures<test_std::set_value_t(throw_on_move),
                                                      test_std::set_error_t(std::exception_ptr)>;
    using storage_t = test_detail::completion_storage<sigs_t>;
    storage_t sut;
    bool      executed{false};
    auto      recv = func_receiver{[&](auto tag, auto data) noexcept {
        ASSERT((std::same_as<decltype(tag), test_std::set_error_t>));
        if constexpr (std::is_same_v<std::remove_cvref_t<decltype(data)>, std::exception_ptr>) {
            try {
                std::rethrow_exception(data);
                ASSERT(false);
            } catch (const std::runtime_error& e) {
                ASSERT(std::string(e.what()) == "throw_on_move");
            }
        } else {
            ASSERT(false);
        }
        executed = true;
    }};

    // Act
    sut.store(test_std::set_value_t{}, throw_on_move{42});
    std::move(sut).complete(std::move(recv));

    // Assert
    ASSERT(executed);
}

/// Checks that when we throw when completing, we send a `set_error` completion with the current exception.
auto test_throw_on_move_when_completing() -> void {
    // Arrange
    using sigs_t    = test_std::completion_signatures<test_std::set_value_t(throw_on_move),
                                                      test_std::set_error_t(std::exception_ptr)>;
    using storage_t = test_detail::completion_storage<sigs_t>;
    storage_t sut;
    bool      executed{false};
    auto      recv = func_receiver{[&](auto tag, auto data) noexcept {
        ASSERT((std::same_as<decltype(tag), test_std::set_error_t>));
        if constexpr (std::is_same_v<std::remove_cvref_t<decltype(data)>, std::exception_ptr>) {
            try {
                std::rethrow_exception(data);
                ASSERT(false);
            } catch (const std::runtime_error& e) {
                ASSERT(std::string(e.what()) == "throw_on_move");
            }
        } else {
            ASSERT(false);
        }
        executed = true;
    }};

    // Act
    sut.store(test_std::set_value_t{}, throw_on_move{42, 2}); // will throw on the second move
    std::move(sut).complete(std::move(recv));

    // Assert
    ASSERT(executed);
}

TEST(exec_completion_storage) {
    test_store_trivial_set_value();
    test_store_trivial_set_error();
    test_store_trivial_set_stopped();
    test_store_with_complex_signatures();
    test_throw_on_move_when_storing();
    test_throw_on_move_when_completing();
}
