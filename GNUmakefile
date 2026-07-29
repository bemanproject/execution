# GNUmakefile
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

MAKEFLAGS+= --no-builtin-rules          # Disable the built-in implicit rules.
MAKEFLAGS+= --no-builtin-variables      # Disable the built-in variable settings.
MAKEFLAGS+= --warn-undefined-variables  # Warn when an undefined variable is referenced.
.SUFFIXES:                              # Disable all suffix rules.
##################################################

ifeq ($(origin CXX),default)
  CXX:= clang++-23
  CC:= clang-23
endif

PRESET?=release
IMAGE?=ghcr.io/bemanproject/infra-containers-clang:latest

_build_path:=build/$(PRESET)

.PHONY: all check distclean format dockerbuild

# default target rule
all: .init compile_commands.json ## Make all with cmake workflow preset
	cmake --workflow --preset $(PRESET)

.init: CMakeUserPresets.json CMakePresets.json CMakeLists.txt ## Configure cmake preset in VERBOSE mode
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
	ln -fs $< $@

distclean: ## Remove all build artifacts
	rm -rf build .cache
	rm -f CMakeUserPresets.json \
	compile_commands.json \
	.init
	find . -name '*~' -delete

format: distclean  ## Format all files with pre-commit
	-pre-commit autoupdate
	pre-commit run --all

dockerbuild: ## Start docker image interactive
	docker run -it -v $(CURDIR):/home/builder/workdir $(IMAGE)

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

# Anything we don't know how to build will use this rule.
% ::
	ninja -C $(_build_path) $(@)
