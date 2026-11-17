# gersemi: off
# cmake/std_par.cmake
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
# gersemi: on

# Defines beman.execution.std_par: an INTERFACE target executables link to when
# libstdc++'s <execution> needs TBB.
if(NOT TARGET beman.execution.std_par)
    include(CheckCXXSourceCompiles)
    check_cxx_source_compiles(
        "#include <algorithm>
         #include <execution>
         #include <vector>
         int main() { std::vector<int> v{2, 1}; std::sort(std::execution::par, v.begin(), v.end()); }"
        BEMAN_EXECUTION_STD_PAR_LINKS
    )
    add_library(beman.execution.std_par INTERFACE)
    if(NOT BEMAN_EXECUTION_STD_PAR_LINKS)
        find_package(TBB REQUIRED)
        target_link_libraries(beman.execution.std_par INTERFACE TBB::tbb)
    endif()
endif()
