#include <beman/execution/execution.hpp>
namespace ex = beman::execution;

struct receiver {
    using receiver_concept = ex::receiver_t;
    auto set_value(auto&&...) noexcept -> void {}
    auto set_error(auto&&) noexcept -> void {}
    auto set_stopped() noexcept -> void {}
};

int main() {
    // ex::sync_wait(ex::just_stopped() | ex::then([]{}));
    auto then = ex::just_stopped() | ex::then([] {});
    static_assert(std::same_as<void, decltype(ex::get_completion_signatures(then, ex::env<>()))>);

    ex::connect(ex::just_stopped() | ex::then([] {}), receiver{});
}
