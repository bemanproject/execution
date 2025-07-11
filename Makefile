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

.PHONY: default release debug doc run update check ce todo distclean clean codespell clang-tidy build test install all format unstage $(SANITIZERS)

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
SANITIZER ?= default
SOURCEDIR = $(CURDIR)
BUILDROOT = build
SYSTEM    = $(shell uname -s)
BUILD     = $(BUILDROOT)/$(SYSTEM)/$(SANITIZER)
EXAMPLE   = beman.execution.examples.stop_token

export CXX=$(COMPILER)

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

default: test

all: $(SANITIZERS)

run: test
	./$(BUILD)/examples/$(EXAMPLE)

doc:
	doxygen docs/Doxyfile

# $(SANITIZERS):
# 	$(MAKE) SANITIZER=$@

build:
	cmake --fresh -G Ninja -S $(SOURCEDIR) -B  $(BUILD) $(TOOLCHAIN) $(SYSROOT) \
	  -D CMAKE_EXPORT_COMPILE_COMMANDS=1 \
	  -D CMAKE_SKIP_INSTALL_RULES=1 \
	  -D CMAKE_CXX_STANDARD=23 \
	  -D CMAKE_CXX_COMPILER=$(CXX) # XXX -D CMAKE_CXX_FLAGS="$(CXX_FLAGS) $(SAN_FLAGS)"
	cmake --build $(BUILD)

# NOTE: without install! CK
test: build
	ctest --test-dir $(BUILD) --rerun-failed --output-on-failure

install: test
	cmake --install $(BUILD) --prefix /opt/local

CMakeUserPresets.json: cmake/CMakeUserPresets.json
	ln -s $< $@

release: CMakeUserPresets.json
	cmake --preset $@ --fresh --log-level=TRACE
	cmake --workflow --preset $@

debug: CMakeUserPresets.json
	cmake --preset $@ --fresh --log-level=TRACE
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
	run-clang-tidy -p $(BUILD) tests examples

codespell:
	codespell -L statics,snd,copyable,cancelled

format: cmake-format clang-format

cmake-format:
	pre-commit run --all

clang-format:
	git clang-format main

todo:
	bin/mk-todo.py

unstage:
	git restore --staged tests/beman/execution/CMakeLists.txt

.PHONY: clean-doc
clean-doc:
	$(RM) -r docs/html docs/latex

clean: clean-doc
	cmake --build $(BUILD) --target clean
	$(RM) mkerr olderr *~

distclean: clean
	$(RM) -r $(BUILDROOT) stagedir
