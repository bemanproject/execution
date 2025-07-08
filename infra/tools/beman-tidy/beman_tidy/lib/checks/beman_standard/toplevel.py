#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

from .cmake import CMakeBaseCheck
from .license import LicenseBaseCheck
from .readme import ReadmeBaseCheck
from ..system.registry import register_beman_standard_check

# [TOPLEVEL.*] checks category.
# All checks in this file extend the ToplevelBaseCheck class.
#
# Note: ToplevelBaseCheck is not a registered check!


@register_beman_standard_check(check="TOPLEVEL.CMAKE")
class ToplevelCmakeCheck(CMakeBaseCheck):
    def __init__(self, repo_info, beman_standard_check_config):
        super().__init__(repo_info, beman_standard_check_config)

    def check(self):
        return super().pre_check()

    def fix(self):
        # TODO: Implement the fix.
        pass


@register_beman_standard_check("TOPLEVEL.LICENSE")
class ToplevelLicenseCheck(LicenseBaseCheck):
    def __init__(self, repo_info, beman_standard_check_config):
        super().__init__(repo_info, beman_standard_check_config)

    def check(self):
        # since this class simply checks for the existence of a LICENSE file,
        # there's nothing more to do than the default pre-check.
        return super().pre_check()

    def fix(self):
        self.log(
            "Please add a LICENSE file to the repository. See https://github.com/bemanproject/beman/blob/main/docs/BEMAN_STANDARD.md#license for more information."
        )


@register_beman_standard_check("TOPLEVEL.README")
class ToplevelReadmeCheck(ReadmeBaseCheck):
    def __init__(self, repo_info, beman_standard_check_config):
        super().__init__(repo_info, beman_standard_check_config)

    def check(self):
        # since this class simply checks for the existence of a README file,
        # there's nothing more to do than the default pre-check.
        return super().pre_check()

    def fix(self):
        self.log(
            "Please write a README file. See https://github.com/bemanproject/beman/blob/main/docs/BEMAN_STANDARD.md#readmemd for the desired format."
        )
