#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

from abc import abstractmethod
import re

from .base_check import BaseCheck


class FileBaseCheck(BaseCheck):
    """
    Base class for checks that operate on a file.
    """

    def __init__(self, repo_info, beman_standard_check_config, relative_path):
        super().__init__(repo_info, beman_standard_check_config)

        # set path - e.g. "README.md"
        self.path = self.repo_path / relative_path

    def pre_check(self):
        """
        Override.
        Pre-checks if the file exists and is not empty.
        """
        if not super().pre_check():
            return False

        if self.path is None:
            self.log("The path is not set.")
            return False

        if not self.path.exists():
            self.log(f"The file '{self.path}' does not exist.")
            return False

        if self.is_empty():
            self.log(f"The file '{self.path}' is empty.")
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

    def read(self):
        """
        Read the file content.
        """
        try:
            with open(self.path, "r") as file:
                return file.read()
        except Exception:
            return ""

    def read_lines(self):
        """
        Read the file content as lines.
        """
        try:
            with open(self.path, "r") as file:
                return file.readlines()
        except Exception:
            return []

    def read_lines_strip(self):
        """
        Read the file content as lines and strip them.
        """
        return [line.strip() for line in self.read_lines()]

    def write(self, content):
        """
        Write the content to the file.
        """
        try:
            with open(self.path, "w") as file:
                file.write(content)
        except Exception as e:
            self.log(f"Error writing the file '{self.path}': {e}")

    def write_lines(self, lines):
        """
        Write the lines to the file.
        """
        self.write("\n".join(lines))

    def replace_line(self, line_number, new_line):
        """
        Replace the line at the given line number with the new line.
        """
        lines = self.read_lines()
        lines[line_number] = new_line
        self.write_lines(lines)

    def is_empty(self):
        """
        Check if the file is empty.
        """
        return len(self.read()) == 0

    def has_content(self, content_to_match):
        """
        Check if the file contains the given content (literal string match).
        """
        readme_content = self.read()
        if not readme_content or len(readme_content) == 0:
            return False
        return re.search(re.escape(content_to_match), readme_content) is not None
