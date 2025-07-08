#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

from ..base.file_base_check import FileBaseCheck

# [CMAKE.*] checks category.
# All checks in this file extend the CMakeBaseCheck class.
#
# Note: CMakeBaseCheck is not a registered check!


class CMakeBaseCheck(FileBaseCheck):
    def __init__(self, repo_info, beman_standard_check_config):
        super().__init__(repo_info, beman_standard_check_config, "CMakeLists.txt")


# TODO CMAKE.DEFAULT


# TODO CMAKE.USE_FETCH_CONTENT


# TODO CMAKE.PROJECT_NAME


# TODO CMAKE.PASSIVE_PROJECTS


# TODO CMAKE.LIBRARY_NAME


# TODO CMAKE.LIBRARY_ALIAS


# TODO CMAKE.TARGET_NAMES


# TODO CMAKE.PASSIVE_TARGETS


# TODO CMAKE.SKIP_TESTS


# TODO CMAKE.SKIP_EXAMPLES


# TODO CMAKE.AVOID_PASSTHROUGHS
