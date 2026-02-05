<!-- SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception -->

# Create and install an interface library and the `CXX_MODULE` if possible

To be able to compile a project with and without MODULES, the project needs at least 2 targets:

- an interface library (header only) with one `PUBLIC FILE_SET HEADERS`
- a library (static and/or dynamic) with one `PUBLIC FILE_SET CXX_MODULE`


## NEW CMAKE modules intended to be in infra/cmake:

- prelude.cmake
- cxx-modules-rules.cmake
- beman-install-library.cmake
- Config.cmake.in


## NEW added related CMake options:

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

if(BEMAN_USE_MODULES AND CMAKE_CXX_MODULE_STD)
    target_compile_definitions(beman.execution PUBLIC BEMAN_HAS_IMPORT_STD)
endif()
```


### beman_install_library

Installs a library (or set of targets) along with headers, C++ modules,
and optional CMake package configuration files.

#### Usage:

  beman_install_library(<name>
    TARGETS target1 [target2 ...]
    DEPENDENCIES dependency1 [dependency2 ...]
    [NAMESPACE <namespace>]
    [EXPORT_NAME <export-name>]
    [DESTINATION <install-prefix>]
  )

#### Arguments:

name
  Logical package name (e.g. "beman.utility").
  Used to derive config file names and cache variable prefixes.

TARGETS (required)
  List of CMake targets to install.

DEPENDENCIES (optional)
  Semicolon-separated list, one dependency per entry.
  Each entry is a valid find_dependency() argument list.
  Note: you must use the bracket form for quoting if not only a package name is used!
  "[===[beman.inplace_vector 1.0.0]===] [===[beman.scope 0.0.1 EXACT]===] fmt"

NAMESPACE (optional)
  Namespace for exported targets.
  Defaults to "beman::".

EXPORT_NAME (optional)
  Name of the CMake export set.
  Defaults to "<name>-targets".

DESTINATION (optional)
  The install destination for CXX_MODULES.
  Defaults to CMAKE_INSTALL_INCLUDEDIR/beman/modules.

#### Brief

This function installs the specified project TARGETS and its FILE_SET
HEADERS to the default CMAKE install Destination.

It also handles the installation of the CMake config package files if
needed.  If the given targets has FILE_SET CXX_MODULE, it will also
installed to the given DESTINATION

#### Cache variables:

BEMAN_INSTALL_CONFIG_FILE_PACKAGES
  List of package names for which config files should be installed.

<PREFIX>_INSTALL_CONFIG_FILE_PACKAGE
  Per-package override to enable/disable config file installation.
  <PREFIX> is the uppercased package name with dots replaced by underscores.


## The possible usage in CMakeLists.txt

    include(./cmake/prelude.cmake)
    project(beman.execution VERSION 0.2.0 LANGUAGES CXX)
    include(./cmake/cxx-modules-rules.cmake)

    # NOTE: this must be done before tests! CK
    include(beman-install-library)
    beman_install_library(${PROJECT_NAME} TARGETS ${PROJECT_NAME} beman.exemplar_headers #
        # TODO(add): DEPENDENCIES [===[beman.inplace_vector 1.0.0]===] [===[beman.scope 0.0.1 EXACT]===] fmt
    )


## Possible cmake config output

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


## Possible cmake export config package

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
