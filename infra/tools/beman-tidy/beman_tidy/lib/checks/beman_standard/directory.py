#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

from ..base.directory_base_check import DirectoryBaseCheck
from ..system.registry import register_beman_standard_check


# [DIRECTORY.*] checks category.
class BemanTreeDirectoryCheck(DirectoryBaseCheck):
    """
    Check if the directory tree is a Beman tree.
    """

    def __init__(self, repo_info, beman_standard_check_config, prefix_path):
        super().__init__(
            repo_info,
            beman_standard_check_config,
            f"{prefix_path}/beman/{repo_info['name']}",
        )


# TODO DIRECTORY.INTERFACE_HEADERS


# TODO DIRECTORY.IMPLEMENTATION_HEADERS


# TODO DIRECTORY.SOURCES
@register_beman_standard_check("DIRECTORY.SOURCES")
class DirectorySourcesCheck(BemanTreeDirectoryCheck):
    """
    Check if the sources directory is src/beman/<short_name>.
    """

    def __init__(self, repo_info, beman_standard_check_config):
        super().__init__(repo_info, beman_standard_check_config, "src")

    def check(self):
        return self.pre_check()

    def fix(self):
        """
        TODO: Implement the fix.
        """
        pass


# TODO DIRECTORY.TESTS


# TODO DIRECTORY.EXAMPLES


# TODO DIRECTORY.DOCS


# TODO DIRECTORY.PAPERS
