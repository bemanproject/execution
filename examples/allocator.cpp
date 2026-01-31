#include <beman/execution/execution.hpp>
#include <iostream>
#include <string>
#include <memory>
#include <memory_resource>
#include <span>
#include <vector>

namespace ex = beman::execution;

namespace {
template <std::size_t Size>
struct inline_resource : std::pmr::memory_resource {
    const char* name;
    explicit inline_resource(const char* n) : name(n) {}
    std::byte  buffer[Size]{};      // NOLINT(hicpp-avoid-c-arrays)
    std::byte* next{+this->buffer}; // NOLINT(hicpp-no-array-decay)

    void* do_allocate(std::size_t size, std::size_t) override {
        std::cout << "allocating from=" << this->name << ", size=" << size << "\n";
        if (size <= std::size_t(std::distance(next, std::end(buffer)))) {
            std::byte* rc{this->next};
            this->next += size;
            return rc;
        }
        return nullptr;
    }
    void do_deallocate(void*, std::size_t, std::size_t) override {}
    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override { return this == &other; }
};

// NOLINTBEGIN(hicpp-special-member-functions)
template <typename Fun>
struct allocator_aware_fun {
    using allocator_type = std::pmr::polymorphic_allocator<>;

    std::remove_cvref_t<Fun> fun;
    allocator_type           allocator{};

    template <typename F>
        requires std::same_as<std::remove_cvref_t<F>, std::remove_cvref_t<Fun>>
    explicit allocator_aware_fun(F&& f) : fun(std::forward<F>(f)) {}
    allocator_aware_fun(const allocator_aware_fun& other, allocator_type alloc = {})
        : fun(other.fun), allocator(alloc) {}
    allocator_aware_fun(allocator_aware_fun&& other) noexcept
        : fun(std::move(other.fun)), allocator(other.allocator) {}
    allocator_aware_fun(allocator_aware_fun&& other, allocator_type alloc)
        : fun(std::move(other.fun)), allocator(alloc) {}

    template <typename... Args>
    auto operator()(Args&&... args) noexcept {
        return this->fun(this->allocator, std::forward<Args>(args)...);
    }
};
// NOLINTEND(hicpp-special-member-functions)
template <typename Fun>
allocator_aware_fun(Fun&& fun) -> allocator_aware_fun<Fun>;

struct allocator_env {
    std::pmr::polymorphic_allocator<> allocator{};
    auto query(ex::get_allocator_t) const noexcept -> std::pmr::polymorphic_allocator<> { return this->allocator; }
};
} // namespace

auto main() -> int {
    int  values[] = {1, 2, 3}; // NOLINT(hicpp-avoid-c-arrays)
    auto s{ex::just(std::span(values)) | ex::let_value(allocator_aware_fun([](auto alloc, std::span<int> v) {
               return ex::just(std::pmr::vector<int>(v.begin(), v.end(), alloc));
           })) |
           ex::then([](auto&& v) noexcept {
               for (auto x : v) {
                   std::cout << x << ", ";
               }
               std::cout << "\n";
           })};

    inline_resource<1024> state_resource("state");
    ex::sync_wait(ex::write_env(std::move(s), allocator_env{&state_resource}));
}
