<!-- SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception -->

# CMake Support for Header-Only Libraries and C++ Modules

This directory provides a small set of CMake helper modules that make it easier
to **build and install a library that works both with and without C++20
modules**.

The helpers wrap CMake’s evolving support for `FILE_SET CXX_MODULE` and package
installation so that:

- consumers can use the library in **classic header-only mode**, or

- consumers with a compatible toolchain can use **C++ modules**, including
  optional `import std;` support.

The goal is to keep project `CMakeLists.txt` files readable while hiding most of
the platform- and compiler-specific complexity.

---

## Motivation

Today, supporting C++ modules often requires duplicating targets and custom
install logic:

- one target for headers only,
- another target that owns module interface units,
- conditional logic depending on compiler and CMake version,
- non-trivial install/export rules.

The helpers in this directory centralize that logic and provide a single,
consistent API for projects that want to offer both usage models.

---

## Provided CMake Modules

The following modules are expected to live in `infra/cmake`:

- `prelude.cmake`
  Common project setup and option handling.

- `cxx-modules-rules.cmake`
  Compiler and CMake feature checks related to C++ modules.

- `beman-install-library.cmake`
  High-level helper to install libraries, headers, module interface units, and CMake package files.

- `Config.cmake.in`
  Template used to generate the `<package>-config.cmake` file for consumers.

---

## New Configuration Options

    * BEMAN_USE_MODULES:BOOL=ON
    * BEMAN_USE_STD_MODULE:BOOL=OFF
    * BEMAN_HAS_IMPORT_STD::BOOL=${CMAKE_CXX_SCAN_FOR_MODULES} # only if toolchain supports it.

### The recommended usage in CMake code

```cmake
if(CMAKE_CXX_STANDARD GREATER_EQUAL 20)
    option(BEMAN_USE_MODULES "Build CXX_MODULES" ${CMAKE_CXX_SCAN_FOR_MODULES})
endif()
if(BEMAN_USE_MODULES)
    target_compile_definitions(beman.execution PUBLIC BEMAN_HAS_MODULES)
endif()

BEMAN_USE_STD_MODULE:BOOL=ON
# -> "Check if 'import std;' is possible with the toolchain?"

if(BEMAN_USE_MODULES AND BEMAN_HAS_IMPORT_STD)
    target_compile_definitions(beman.execution PUBLIC BEMAN_HAS_IMPORT_STD)
    set_target_properties(beman.execution PROPERTIES CXX_MODULE_STD ON)
endif()
```

Typical projects will only toggle `BEMAN_USE_MODULES`; the remaining options are
detected automatically.

---

## Installing a Library

Installation is handled by the `beman_install_library()` helper.

### Function Signature

```cmake
beman_install_library(name
    TARGETS <target1> [<target2> ...]
    [DEPENDENCIES <dep1> [<dep2> ...]]
    [NAMESPACE <namespace>]
    [EXPORT_NAME <export-name>]
    [DESTINATION <module-destination>]
)
```

### Arguments

- **name**
  Logical package name (e.g. "beman.utility").
  Used to derive config file names and cache variable prefixes.

- **TARGETS**
  Targets to install and export.

- **DEPENDENCIES (optional)**
  Semicolon-separated list, one dependency per entry.
  Each entry is a valid `find_dependency()` argument list.

  **NOTE:** you must use the bracket form for quoting if not only a package name is used!

  `"[===[beman.inplace_vector 1.0.0]===] [===[beman.scope 0.0.1 EXACT]===] fmt"`

- **NAMESPACE (optional)**
  Namespace for imported targets.
  Defaults to "beman::".

- **EXPORT_NAME (optional)**
  Name of the generated CMake export set.
  Defaults to "<name>-targets".

- **DESTINATION (optional)**
  Installation directory for C++ module interface units.
  Defaults to `${CMAKE_INSTALL_INCLUDEDIR}/beman/modules`.

This function installs the specified project TARGETS and its `FILE_SET
TYPE HEADERS` to the default CMAKE install destination.

It also handles the installation of the CMake config package files if
needed.  If the given targets has `FILE_SET TYPE CXX_MODULE`, it will also
installed to the given DESTINATION

- **Used Cache variables**

`BEMAN_INSTALL_CONFIG_FILE_PACKAGES`
  List of package names for which config files should be installed.

`<PREFIX>_INSTALL_CONFIG_FILE_PACKAGE`
  Per-package override to enable/disable config file installation.
  <PREFIX> is the uppercased package name with dots replaced by underscores.

### Caveats

- **Only one `FILE_SET CXX_MODULES` is yet supported to install with this
  function!**

- **Only header files contained in a `PUBLIC FILE_SET TYPE HEADERS` will be
  installed with this function!**

---

## The possible usage in CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.30...4.2)

include(./cmake/prelude.cmake)
project(beman.execution VERSION 0.2.0 LANGUAGES CXX)
include(./cmake/cxx-modules-rules.cmake)

set(BEMAN_EXECUTION_TARGET_PREFIX ${PROJECT_NAME})

#===============================================================================
if(BEMAN_USE_MODULES)
    set(CMAKE_CXX_VISIBILITY_PRESET hidden)
    set(CMAKE_VISIBILITY_INLINES_HIDDEN TRUE)

    # CMake requires the language standard to be specified as compile feature
    # when a target provides C++23 modules and the target will be installed
    add_library(${BEMAN_EXECUTION_TARGET_PREFIX} STATIC)
    add_library(beman::execution ALIAS ${BEMAN_EXECUTION_TARGET_PREFIX})
    target_compile_features(
        ${BEMAN_EXECUTION_TARGET_PREFIX}
        PUBLIC cxx_std_${CMAKE_CXX_STANDARD}
    )

    include(GenerateExportHeader)
    generate_export_header(
        ${BEMAN_EXECUTION_TARGET_PREFIX}
        BASE_NAME ${BEMAN_EXECUTION_TARGET_PREFIX}
        EXPORT_FILE_NAME beman/execution/modules_export.hpp
    )
    target_sources(
        ${BEMAN_EXECUTION_TARGET_PREFIX}
        PUBLIC
            FILE_SET HEADERS
                BASE_DIRS include ${CMAKE_CURRENT_BINARY_DIR}
                FILES
                    ${CMAKE_CURRENT_BINARY_DIR}/beman/execution/modules_export.hpp
    )
    target_compile_definitions(${BEMAN_EXECUTION_TARGET_PREFIX} PUBLIC BEMAN_HAS_MODULES)
endif()

if(BEMAN_USE_MODULES AND CMAKE_CXX_MODULE_STD)
    target_compile_definitions(${BEMAN_EXECUTION_TARGET_PREFIX} PUBLIC BEMAN_HAS_IMPORT_STD)
else()
    message(WARNING "Missing support for CMAKE_CXX_MODULE_STD!")
endif()
#===============================================================================

# ...

# NOTE: this must be done before tests! CK
include(beman-install-library)
beman_install_library(${PROJECT_NAME} TARGETS ${BEMAN_EXECUTION_TARGET_PREFIX} beman.exemplar_headers #
    # TODO(add): DEPENDENCIES [===[beman.inplace_vector 1.0.0]===] [===[beman.scope 0.0.1 EXACT]===] fmt
)
```

---

## Possible cmake config output

**NOTE:** Exact output depend on the build host, used toolchain, and
whether module support is enabled and supported.

```bash
bash-5.3$ make test-module
cmake -G Ninja -S /Users/clausklein/Workspace/cpp/beman-project/execution26 -B build/Darwin/default   \
	  -D CMAKE_EXPORT_COMPILE_COMMANDS=ON \
	  -D CMAKE_SKIP_INSTALL_RULES=OFF \
	  -D CMAKE_CXX_STANDARD=23 \
	  -D CMAKE_CXX_EXTENSIONS=ON \
	  -D CMAKE_CXX_STANDARD_REQUIRED=ON \
	  -D CMAKE_CXX_SCAN_FOR_MODULES=ON \
	  -D BEMAN_USE_MODULES=ON \
	  -D CMAKE_BUILD_TYPE=Release \
	  -D CMAKE_INSTALL_MESSAGE=LAZY \
	  -D CMAKE_BUILD_TYPE=Release \
	  -D CMAKE_CXX_COMPILER=clang++ --log-level=VERBOSE
-- use ccache
-- CXXFLAGS=-stdlib=libc++
'brew' '--prefix' 'llvm'
-- LLVM_DIR=/usr/local/Cellar/llvm/21.1.8
-- CMAKE_CXX_STDLIB_MODULES_JSON=/usr/local/Cellar/llvm/21.1.8/lib/c++/libc++.modules.json
-- CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES=/usr/local/Cellar/llvm/21.1.8/include/c++/v1;/usr/local/Cellar/llvm/21.1.8/lib/clang/21/include;/Library/Developer/CommandLineTools/SDKs/MacOSX14.sdk/usr/include
-- CMAKE_CXX_STDLIB_MODULES_JSON=/usr/local/Cellar/llvm/21.1.8/lib/c++/libc++.modules.json
-- BEMAN_USE_STD_MODULE=ON
-- CMAKE_CXX_COMPILER_IMPORT_STD=23;26
-- CMAKE_CXX_MODULE_STD=ON
-- BEMAN_HAS_IMPORT_STD=ON
-- BEMAN_USE_MODULES=ON
-- CMAKE_CXX_SCAN_FOR_MODULES=ON
-- Performing Test COMPILER_HAS_HIDDEN_VISIBILITY
-- Performing Test COMPILER_HAS_HIDDEN_VISIBILITY - Success
-- Performing Test COMPILER_HAS_HIDDEN_INLINE_VISIBILITY
-- Performing Test COMPILER_HAS_HIDDEN_INLINE_VISIBILITY - Success
-- Performing Test COMPILER_HAS_DEPRECATED_ATTR
-- Performing Test COMPILER_HAS_DEPRECATED_ATTR - Success
-- beman_install_library(beman.execution): COMPONENT execution_headers for TARGET 'beman.execution_headers'
-- beman-install-library(beman.execution): 'beman.execution_headers' has INTERFACE_HEADER_SETS=public_headers
-- beman_install_library(beman.execution): COMPONENT execution for TARGET 'beman.execution'
-- beman-install-library(beman.execution): 'beman.execution' has INTERFACE_HEADER_SETS=HEADERS
-- beman-install-library(beman.execution): 'beman.execution' has CXX_MODULE_SETS=CXX_MODULES
-- Configuring done (3.1s)
CMake Warning (dev) in CMakeLists.txt:
  CMake's support for `import std;` in C++23 and newer is experimental.  It
  is meant only for experimentation and feedback to CMake developers.
This warning is for project developers.  Use -Wno-dev to suppress it.

-- Generating done (0.4s)
-- Build files have been written to: /Users/clausklein/Workspace/cpp/beman-project/execution26/build/Darwin/default
bash-5.3$
```

---

## Possible cmake export config package

**NOTE:** Exact contents depend on the build host, used toolchain, and
whether module support is enabled.

```bash
cmake --install build/Darwin/default --prefix $PWD/stagedir

bash-5.3$ cd stagedir/
bash-5.3$ tree lib/
lib/
├── cmake
│   └── beman.execution
│       ├── beman.execution-config-version.cmake
│       ├── beman.execution-config.cmake
│       ├── beman.execution-targets-debug.cmake
│       ├── beman.execution-targets-relwithdebinfo.cmake
│       ├── beman.execution-targets.cmake
│       ├── bmi-GNU_Debug
│       │   └── beman.execution.gcm
│       ├── bmi-GNU_RelWithDebInfo
│       │   └── beman.execution.gcm
│       ├── cxx-modules
│       │   ├── cxx-modules-beman.execution-Debug.cmake
│       │   ├── cxx-modules-beman.execution-RelWithDebInfo.cmake
│       │   ├── cxx-modules-beman.execution.cmake
│       │   ├── target-execution-Debug.cmake
│       │   └── target-execution-RelWithDebInfo.cmake
│       └── modules
│           └── execution.cppm
├── libbeman.execution.a
└── libbeman.execution_d.a

7 directories, 15 files
bash-5.3$
```

---

## The recommended usage in implementation

```cpp
// identity.cppm
module;

#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <utility>
#endif

export module beman.exemplar;

export namespace beman::exemplar {
struct __is_transparent;

struct identity {
    template <class T>
    constexpr T&& operator()(T&& t) const noexcept {
        return std::forward<T>(t);
    }

    using is_transparent = __is_transparent;
};
} // namespace beman::exemplar
```

---

## The possible usage in user code

```cpp
// example-usage.cpp

#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <iostream>
#endif

#ifdef BEMAN_HAS_MODULES
import beman.exemplar;
#else
#include <beman/exemplar/identity.hpp>
#endif

int main() {
    beman::exemplar::identity id;

    int x = 42;
    int y = id(x); // y == 42

    std::cout << y << '\n';
    return 0;
}
```

---

## Notes on CMake and Modules

- `FILE_SET CXX_MODULE` support is still evolving and depends on the compiler
  and CMake version.

- Not all toolchains support installing or consuming prebuilt module interface
  units yet.

- These helpers aim to provide a pragmatic, forward-compatible structure rather
  than a final standard solution.

---

## Summary

The helpers in this directory provide a structured way to:

- build header-only and module-based libraries side by side,
- hide compiler- and platform-specific complexity,
- install and export targets in a consistent, consumer-friendly way.

They are intended for projects that want to experiment with C++ modules today
while remaining usable on traditional toolchains.
