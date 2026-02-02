# Makefile
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

MAKEFLAGS+= --no-builtin-rules          # Disable the built-in implicit rules.
MAKEFLAGS+= --warn-undefined-variables  # Warn when an undefined variable is referenced.

SANITIZERS := run
# SANITIZERS = usan # TODO: lsan
# OS := $(shell /usr/bin/uname)
# ifeq ($(OS),Darwin)
#     SANITIZERS += tsan # TODO: asan
# endif
# ifeq ($(OS),Linux)
#     SANITIZERS += asan # TODO: tsan msan
# endif

.PHONY: default release debug doc run update check ce todo distclean clean codespell clang-tidy build test install all format unstage $(SANITIZERS) module build-module test-module

SYSROOT   ?=
TOOLCHAIN ?=

COMPILER=c++
CXX_BASE=$(CXX:$(dir $(CXX))%=%)
ifeq ($(CXX_BASE),g++)
    COMPILER=g++
endif
ifeq ($(CXX_BASE),clang++)
    COMPILER=clang++
endif

LDFLAGS   ?=
SAN_FLAGS ?=
CXX_FLAGS ?= -g
# TODO: SANITIZER := release
SANITIZER ?= default
SOURCEDIR = $(CURDIR)
BUILDROOT = build
export hostSystemName:=$(shell uname -s)
# TODO BUILD     := $(BUILDROOT)/$(SANITIZER)
BUILD     ?= $(BUILDROOT)/$(hostSystemName)/$(SANITIZER)
EXAMPLE   = beman.execution.examples.stop_token

################################################
ifeq (${hostSystemName},Darwin)
  export LLVM_PREFIX:=$(shell brew --prefix llvm)
  export LLVM_DIR:=$(shell realpath ${LLVM_PREFIX})
  export PATH:=${LLVM_DIR}/bin:${PATH}

  # export CMAKE_CXX_STDLIB_MODULES_JSON=${LLVM_DIR}/lib/c++/libc++.modules.json
  # export CXX=clang++
  # export LDFLAGS=-L$(LLVM_DIR)/lib/c++ -lc++abi -lc++ # -lc++experimental
  # export GCOV="llvm-cov gcov"

  ### TODO: to test g++-15:
  #  export GCC_PREFIX:=$(shell brew --prefix gcc)
  #  export GCC_DIR:=$(shell realpath ${GCC_PREFIX})

  # XXX export CMAKE_CXX_STDLIB_MODULES_JSON=${GCC_DIR}/lib/gcc/current/libstdc++.modules.json
  ifeq ($(CXX),)
    export CXX=g++-15
    export CXXFLAGS=-stdlib=libstdc++
  endif
  export GCOV="gcov"
else ifeq (${hostSystemName},Linux)
  export LLVM_DIR=/usr/lib/llvm-20
  export PATH:=${LLVM_DIR}/bin:${PATH}
  # export CXX=clang++-20
else
  export CXX=$(COMPILER)
endif
################################################

ifeq ($(SANITIZER),release)
    CXX_FLAGS = -O3 -Wpedantic -Wall -Wextra -Wno-shadow -Werror
endif
ifeq ($(SANITIZER),debug)
    CXX_FLAGS = -g
endif
ifeq ($(SANITIZER),msan)
    SAN_FLAGS = -fsanitize=memory
    LDFLAGS = $(SAN_FLAGS)
endif
ifeq ($(SANITIZER),asan)
    SAN_FLAGS = -fsanitize=address -fsanitize=pointer-compare -fsanitize=pointer-subtract -fsanitize-address-use-after-scope
endif
ifeq ($(SANITIZER),usan)
    SAN_FLAGS = -fsanitize=undefined
    LDFLAGS = $(SAN_FLAGS)
endif
ifeq ($(SANITIZER),tsan)
    SAN_FLAGS = -fsanitize=thread
    LDFLAGS = $(SAN_FLAGS)
endif
ifeq ($(SANITIZER),lsan)
    SAN_FLAGS = -fsanitize=leak
    LDFLAGS = $(SAN_FLAGS)
endif

# TODO: beman.execution.examples.modules
# FIXME: beman.execution.execution-module.test beman.execution.stop-token-module.test

default: release

all: $(SANITIZERS)

run: test
	./$(BUILD)/examples/$(EXAMPLE)

doc:
	./bin/mk-doc.py docs/*.mds
	doxygen docs/Doxyfile

# $(SANITIZERS):
# 	$(MAKE) SANITIZER=$@

# ==========================================================
# NOTE: cmake configure to only test without modules! CK
# ==========================================================
build:
	cmake -G Ninja -S $(SOURCEDIR) -B $(BUILD) $(TOOLCHAIN) $(SYSROOT) \
	  -D CMAKE_EXPORT_COMPILE_COMMANDS=ON \
	  -D CMAKE_SKIP_INSTALL_RULES=ON \
	  -D CMAKE_CXX_STANDARD=23 \
	  -D CMAKE_CXX_EXTENSIONS=ON \
	  -D CMAKE_CXX_STANDARD_REQUIRED=ON \
	  -D CMAKE_CXX_SCAN_FOR_MODULES=OFF \
	  -D BEMAN_USE_MODULES=OFF \
	  -D CMAKE_BUILD_TYPE=Release \
	  -D CMAKE_CXX_COMPILER=$(CXX) --log-level=VERBOSE
	cmake --build $(BUILD)
# XXX --fresh -D CMAKE_CXX_FLAGS="$(CXX_FLAGS) $(SAN_FLAGS)"

# NOTE: without install, see CMAKE_SKIP_INSTALL_RULES! CK
test: build
	ctest --test-dir $(BUILD) --rerun-failed --output-on-failure

module build-module:
	cmake -G Ninja -S $(SOURCEDIR) -B $(BUILD) $(TOOLCHAIN) $(SYSROOT) \
	  -D CMAKE_EXPORT_COMPILE_COMMANDS=ON \
	  -D CMAKE_SKIP_INSTALL_RULES=ON \
	  -D CMAKE_CXX_STANDARD=23 \
	  -D CMAKE_CXX_EXTENSIONS=ON \
	  -D CMAKE_CXX_STANDARD_REQUIRED=ON \
	  -D CMAKE_CXX_SCAN_FOR_MODULES=ON \
	  -D BEMAN_USE_MODULES=ON \
	  -D CMAKE_BUILD_TYPE=Release \
	  -D CMAKE_CXX_COMPILER=$(CXX) --log-level=VERBOSE
	cmake --build $(BUILD)

test-module: build-module
	ctest --test-dir $(BUILD) --rerun-failed --output-on-failure

install: test
	cmake --install $(BUILD) --prefix /opt/local

CMakeUserPresets.json:: cmake/CMakeUserPresets.json
	ln -s $< $@

# ==========================================================
appleclang-release llvm-release release: #-dk: I'm not sure why that is needed: CMakeUserPresets.json
	cmake --preset $@ --log-level=TRACE # XXX --fresh
	ln -fs $(BUILDROOT)/$@/compile_commands.json .
	cmake --workflow --preset $@

# ==========================================================
appleclang-debug llvm-debug debug: #-dk: I'm not sure why that is needed: CMakeUserPresets.json
	cmake --preset $@ --log-level=TRACE # XXX --fresh
	ln -fs $(BUILDROOT)build/$@/compile_commands.json .
	cmake --workflow --preset $@

ce:
	@mkdir -p $(BUILD)
	bin/mk-compiler-explorer.py $(BUILD)

SOURCE_CMAKELISTS = src/beman/execution/CMakeLists.txt
update:
	bin/update-cmake-headers.py $(SOURCE_CMAKELISTS)

check:
	@for h in `find include -name \*.hpp`; \
	do \
		from=`echo -n $$h | sed -n 's@.*Beman/\(.*\).hpp.*@\1@p'`; \
		< $$h sed -n "/^ *# *include <Beman\//s@.*[</]Beman/\(.*\).hpp>.*@$$from \1@p"; \
	done | tsort > /dev/null

build/$(SANITIZER)/compile_commands.json: $(SANITIZER)

clang-tidy: $(BUILD)/compile_commands.json
	ln -fs $< .
	run-clang-tidy -p $(BUILD) tests examples

codespell:
	pre-commit run $@

# ==========================================================
format:
	pre-commit autoupdate
	pre-commit run --all
# ==========================================================

cmake-format:
	pre-commit run gersemi

clang-format:
	# pre-commit run $@
	git clang-format main

todo:
	bin/mk-todo.py

unstage:
	git restore --staged tests/beman/execution/CMakeLists.txt docs/code/CMakeLists.txt

.PHONY: clean-doc
clean-doc:
	$(RM) -r docs/html docs/latex

clean: clean-doc
	-cmake --build $(BUILD) --target clean
	$(RM) mkerr olderr compile_commands.json

distclean: clean
	$(RM) -r $(BUILDROOT) stagedir CMakeUserPresets.json
	find . -name '*~' -delete

Makefile :: ;
*.txt :: ;
*.json :: ;

# Anything we don't know how to build will use this rule.
% ::
	ninja -C $(BUILD) $(@)
