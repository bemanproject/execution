#include <beman/execution/execution.hpp>
#include <type_traits>
#include <string>
#include <memory_resource>
#include <utility>
#include <iostream>

namespace ex = beman::execution;

template <typename Receiver>
struct just_op_state {
    using operation_state_concept = ex::operation_state_t;
    std::remove_cvref_t<Receiver> rec;
    std::pmr::string              value;

    template <typename R>
    just_op_state(R&& r, std::pmr::string&& val)
        : rec(std::forward<R>(r)), value(std::move(val), ex::get_allocator(ex::get_env(rec))) {}

    void start() & noexcept { ex::set_value(std::move(rec), std::move(value)); }
};

struct test_receiver {
    using receiver_concept = ex::receiver_t;
    auto set_value(auto&&...) && noexcept -> void { std::cout << "set_value\n"; }
    auto set_error(auto&&) && noexcept -> void { std::cout << "set_error\n"; }
    auto set_stopped() && noexcept -> void { std::cout << "set_stopped\n"; }
};

static_assert(ex::receiver<test_receiver>);
static_assert(ex::operation_state<just_op_state<test_receiver>>);

template <typename T>
struct just_sender {
    using sender_concept        = ex::sender_t;
    using completion_signatures = ex::completion_signatures<ex::set_value_t(T)>;

    T value;
    template <ex::receiver Receiver>
    auto connect(Receiver&& r) && -> just_op_state<Receiver> {
        return {std::forward<Receiver>(r), std::move(value)};
    }
};
template <typename T>
just_sender(T&&) -> just_sender<std::remove_cvref_t<T>>;

static_assert(ex::sender<just_sender<std::pmr::string>>);
static_assert(ex::sender_in<just_sender<std::pmr::string>>);

int main() {
    try {
        auto j = just_sender{std::pmr::string("value")};
        auto t = std::move(j) | ex::then([](const std::pmr::string& v) { return v + " then"; });
        auto w = ex::when_all(std::move(t));
        auto e = ex::detail::write_env(std::move(w),
                                       ex::detail::make_env(ex::get_allocator, std::pmr::polymorphic_allocator<>()));

        std::cout << "before start\n";
        auto r = ex::sync_wait(std::move(e));
        if (r) {
            auto [v] = *r;
            std::cout << "produced='" << v << "'\n";
        } else
            std::cout << "operation was cancelled\n";
        std::cout << "after start\n";
    } catch (const std::exception& ex) {
        std::cout << "ERROR: " << ex.what() << "\n";
    }
}
