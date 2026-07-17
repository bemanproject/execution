# GNUmakefile
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

MAKEFLAGS+= --no-builtin-rules          # Disable the built-in implicit rules.
MAKEFLAGS+= --warn-undefined-variables  # Warn when an undefined variable is referenced.
.SUFFIXES:                              # Disable all suffix rules.
##################################################

.PHONY: all check distclean dockerbuild

# default target rule
all: .init compile_commands.json
	cmake --workflow --preset release

.init: CMakeUserPresets.json CMakePresets.json CMakeLists.txt #NO! $(MAKEFILE)
	cmake --preset release --fresh --log-level=VERBOSE
	touch $@

check: .init compile_commands.json
compile_commands.json: build/release/compile_commands.json
	ln -fs $< $@

CMakeUserPresets.json:: cmake/CMakeUserPresets.json
	ln -fs $< $@

distclean:
	rm -rf build .cache
	rm -f CMakeUserPresets.json \
	compile_commands.json \
	.init
	find . -name '*~' -delete

dockerbuild:
	docker run -it -v $(CURDIR):/home/builder/workdir linux-clang:23

# NOTE: double-colon targets which has no prerequisites must always remade by gmake?
#  Prerequisite 'GNUmakefile' is newer than target '.init'.
#  -> Must remake target '.init'.
# NOTE: impizit handled by gmake! GNUmakefile :: ;

# Anything we don't know how to build will use this rule.
% ::
	ninja -C build/release $(@)
