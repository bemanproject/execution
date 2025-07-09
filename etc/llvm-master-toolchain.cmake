set(LLVM_ROOT "$ENV{LLVM_ROOT}" CACHE PATH "Path to LLVM installation")

set(CMAKE_C_COMPILER ${LLVM_ROOT}/bin/clang)
set(CMAKE_CXX_COMPILER ${LLVM_ROOT}/bin/clang++)

set(CMAKE_CXX_FLAGS
    "-std=c++2a \
   -Wall -Wextra \
   -stdlib=libc++"
    CACHE STRING
    "CXX_FLAGS"
    FORCE
)

set(CMAKE_EXE_LINKER_FLAGS
    "-Wl,-rpath,${LLVM_ROOT}/lib"
    CACHE STRING
    "CMAKE_EXE_LINKER_FLAGS"
    FORCE
)

set(CMAKE_CXX_FLAGS_DEBUG
    "-O0 -fno-inline -g3"
    CACHE STRING
    "C++ DEBUG Flags"
    FORCE
)
set(CMAKE_CXX_FLAGS_RELEASE
    "-Ofast -g0 -DNDEBUG"
    CACHE STRING
    "C++ Release Flags"
    FORCE
)
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO
    "-O3 -g -DNDEBUG"
    CACHE STRING
    "C++ Release Flags"
    FORCE
)
set(CMAKE_CXX_FLAGS_TSAN
    "-O3 -g -DNDEBUG -fsanitize=thread"
    CACHE STRING
    "C++ TSAN Flags"
    FORCE
)
set(CMAKE_CXX_FLAGS_ASAN
    "-O3 -g -DNDEBUG -fsanitize=address -fsanitize=undefined -fsanitize=leak"
    CACHE STRING
    "C++ ASAN Flags"
    FORCE
)
