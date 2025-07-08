#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

from abc import ABC, abstractmethod
from pathlib import Path

from ..system.registry import get_beman_standard_check_name_by_class


class BaseCheck(ABC):
    """
    Base class for checks.
    This class is not meant to be used directly, it's meant to be subclassed.
    e.g., check for repository name, check for changelog, check for license, etc.
    """

    def __init__(self, repo_info, beman_standard_check_config, name=None):
        """
        Create a new check instance.
        """

        # check name -  e.g. "README.TITLE"
        self.name = (
            name
            if name is not None
            else get_beman_standard_check_name_by_class(self.__class__)
        )
        assert self.name is not None, (
            f"Check name not found for class: {self.__class__.__name__}"
        )

        # save the check config
        self.config = beman_standard_check_config[self.name]

        # set type - e.g. "REQUIREMENT" or "RECOMMENDATION"
        self.type = beman_standard_check_config[self.name]["type"]
        assert self.type in ["REQUIREMENT", "RECOMMENDATION"], (
            f"Invalid check type: {self.type} for check = {self.name}."
        )

        # set full text body - e.g. "The README.md should begin ..."
        self.full_text_body = beman_standard_check_config[self.name]["full_text_body"]
        assert self.full_text_body is not None

        # set log level - e.g. "ERROR" or "WARNING"
        self.log_level = "ERROR" if self.type == "REQUIREMENT" else "WARNING"
        self.log_enabled = False

        # set repo info
        self.repo_info = repo_info
        assert "name" in repo_info
        self.repo_name = repo_info["name"]
        assert "top_level" in repo_info
        self.repo_path = Path(repo_info["top_level"])
        assert self.repo_path is not None
        self.library_name = f"beman.{self.repo_name}"
        assert self.library_name is not None

        # set beman library maturity model
        beman_library_maturity_model = beman_standard_check_config[
            "README.LIBRARY_STATUS"
        ]
        assert "values" in beman_library_maturity_model
        assert len(beman_library_maturity_model["values"]) == 4
        self.beman_library_maturity_model = beman_library_maturity_model["values"]

    def pre_check(self):
        """
        Pre-checks if this rule is properly initialized.
        Usually, this is internal use only.

        Note: This method is internally called by the framework.
        """
        if self.name is None:
            self.log("The name is not set.")
            return False

        if self.repo_name is None:
            self.log(f"The repo_name is not set for check = {self.name}.")
            return False

        if not self.repo_path:
            self.log(f"The repo_path is not set for check = {self.name}.")
            return False

        return True

    @abstractmethod
    def check(self):
        """
        Checks if the Beman Standard check is already applied.
        - If it's applied, this method should return True.
        - Otherwise, it returns False and self.fix() must be able to fix the issue.

        Note: This methods must be always implemented.
        """
        pass

    @abstractmethod
    def fix(self):
        """
        Fixes the issue if the Beman Standard is not applied.
        - If check already applied, this method is a no-op and should return True.
        - Otherwise, it will try to apply the check inplace. Returns the status of the fix attempt.

        Note: The subclasses might not implement more than a stub if the fix method
        is too difficult to implement or does not make sense.
        """
        pass

    def log(self, message, enabled=True):
        """
        Logs a message with the check's log level.
        e.g. [WARN][REPOSITORY.NAME]: The name "${name}" should be snake_case.'
        e.g. [ERROR][TOPLEVEL.CMAKE]: Missing top level CMakeLists.txt.'
        """

        if self.log_enabled and enabled:
            print(f"[{self.log_level:<15}][{self.name:<25}]: {message}")
