#include <beman/execution/execution.hpp>
#include <cassert>

namespace ex = beman::execution;

namespace {
struct move_only_type {
    move_only_type() : val(0) {}
    explicit move_only_type(int v) : val(v) {}
    ~move_only_type() = default;

    move_only_type(const move_only_type&)            = delete;
    move_only_type& operator=(const move_only_type&) = delete;

    move_only_type& operator=(move_only_type&&) = default;
    move_only_type(move_only_type&&)            = default;
    int val; // NOLINT
};
} // namespace

int main() {
    auto snd = ex::just(move_only_type(1))                                      //
               | ex::then([](move_only_type v) noexcept { return v.val * 2; })  //
               | ex::let_value([](int v) noexcept { return ex::just(v * 3); }); // int
    auto [ret] = ex::sync_wait(std::move(snd)).value();
    assert(ret == 6);

    // NOTE: Compile time error with move_only_type
    auto snd2 = ex::just(move_only_type(1))                                                     //
                | ex::then([](move_only_type v) noexcept { return move_only_type{v.val * 2}; }) //
                | ex::let_value([](move_only_type v) noexcept { return ex::just(v.val * 3); }); // move_only_type
    auto [ret2] = ex::sync_wait(std::move(snd2)).value();
    assert(ret2 == 6);
}
