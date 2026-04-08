#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <iostream>
#include <new>
#include <memory>
#endif
#include <beman/execution/execution.hpp>

namespace ex = beman::execution;

void* operator new(std::size_t n) {
    auto p = std::malloc(n);
    std::cout << "global new(" << n << ")->" << p << "\n";
    return p;
}
void operator delete(void* ptr) noexcept {
    std::cout << "global operator delete()" << ptr << "\n";
}

int main() {
    struct resource: std::pmr::memory_resource {
        void* do_allocate(std::size_t n, std::size_t) override {  return std::malloc(n); }
        void do_deallocate(void* p, std::size_t n, std::size_t) override { std::free(p); } 
        bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override{
            return this == &other;
        }
    } res{};

    ex::sync_wait(
        ex::write_env(
            std::suspend_never(),
            ex::env{ex::prop{ex::get_allocator, std::pmr::polymorphic_allocator<std::byte>(&res)}}
        )
    );
}
