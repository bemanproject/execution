# GNUmakefile
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

MAKEFLAGS+= --no-builtin-rules          # Disable the built-in implicit rules.
MAKEFLAGS+= --no-builtin-variables      # Disable the built-in variable settings.
MAKEFLAGS+= --warn-undefined-variables  # Warn when an undefined variable is referenced.
.SUFFIXES:                              # Disable all suffix rules.
##################################################

ifeq ($(origin CXX),default)
  export CXX:= clang++-23
  export CC:= clang-23
  export CXXFLAGS:= -stdlib=libc++ -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0
endif

PRESET?=release
IMAGE?=ghcr.io/bemanproject/infra-containers-clang:23

_build_path:=build/$(PRESET)

.PHONY: all check distclean format dockerbuild

# default target rule
all: .init compile_commands.json ## Make all with cmake workflow preset
	cmake --workflow --preset $(PRESET)

.init: CMakeUserPresets.json CMakePresets.json CMakeLists.txt ## Configure cmake preset in VERBOSE mode
	cmake --version
	cmake --preset $(PRESET) --fresh --log-level=VERBOSE
	touch $@

check: .init compile_commands.json ## Run clang-tidy on examples
	run-clang-tidy examples

.PHONY: compile_commands.json
compile_commands.json: $(_build_path)/compile_commands.json
	if [ "X$(shell readlink compile_commands.json)" != "X$(_build_path)/compile_commands.json" ] ; then \
		ln -fs $< $@; \
	fi

CMakeUserPresets.json:: cmake/CMakeUserPresets.json
	-ln -fs $< $@

find_package_test: # XXX install  ## Test with installed cmake config package
	cmake -S examples -B build/find_package_test -G Ninja \
    -D CMAKE_CXX_STANDARD=26 \
    -D CMAKE_BUILD_TYPE=Release \
    -D CMAKE_SKIP_TEST_ALL_DEPENDENCY=OFF \
    --debug-find-pkg=beman.execution
	ninja -C build/find_package_test test
	ninja -C build/find_package_test ninja -t deps | egrep -w '(cmake_cxx_std|beman)'
	ninja -C build/find_package_test -t commands beman_execution.example.modules | tail -1

distclean: ## Remove all build artifacts
	rm -rf build .cache
	rm -f CMakeUserPresets.json \
	compile_commands.json \
	.init
	find . -name '*~' -delete

format: distclean  ## Format all files with pre-commit
	git ls-files ':(top,exclude)infra/cmake/' ::cmake.in ::*.cmake ::*CMakeLists.txt | xargs gersemi -i
	# -pre-commit autoupdate --freeze
	# pre-commit run --all

dockerbuild: ## Start docker image interactive
	docker run -it -v $(CURDIR):/src $(IMAGE)

# Helper targets
.PHONY: env info

env: ## Show env
	$(foreach v, $(.VARIABLES), $(info $(v) = $($(v))))

info: ## Show this help.
	@awk 'BEGIN {FS = ":.*?## "} /^[.a-zA-Z_-]+:.*?## / {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}' $(MAKEFILE_LIST) | sort

# NOTE: double-colon targets which has no prerequisites must always remade by gmake?
#  Prerequisite 'GNUmakefile' is newer than target '.init'.
#  -> Must remake target '.init'.
# NOTE: impizit handled by gmake! GNUmakefile :: ;
# NO if exists TODO(CK): cmake/CMakeUserPresets.json :: ;

# Anything we don't know how to build will use this rule.
% ::
	ninja -C $(_build_path) $(@)
