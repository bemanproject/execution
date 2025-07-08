#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

from abc import abstractmethod
from pathlib import Path

from .base_check import BaseCheck


class DirectoryBaseCheck(BaseCheck):
    """
    Base class for checks that operate on a directory.
    """

    def __init__(self, repo_info, beman_standard_check_config, relative_path):
        super().__init__(repo_info, beman_standard_check_config)

        # set path - e.g. "src/beman/exemplar"
        self.path = self.repo_path / relative_path

    def pre_check(self):
        """
        Override.
        Pre-checks if the directory exists and is not empty.
        """
        if not super().pre_check():
            return False

        if self.path is None:
            self.log("The path is not set.")
            return False

        if not self.path.exists():
            self.log(f"The directory '{self.path}' does not exist.")
            return False

        if self.is_empty():
            self.log(f"The directory '{self.path}' is empty.")
            return False

        return True

    @abstractmethod
    def check(self):
        """
        Override this method, make it abstract because this is style an abstract class.
        """
        pass

    @abstractmethod
    def fix(self):
        """
        Override this method, make it abstract because this is style an abstract class.
        """
        pass

    def read(self) -> list[Path]:
        """
        Read the directory content.
        """
        try:
            return list(self.path.iterdir())
        except Exception:
            return []

    def is_empty(self):
        """
        Check if the directory is empty.
        """
        return len(self.read()) == 0
