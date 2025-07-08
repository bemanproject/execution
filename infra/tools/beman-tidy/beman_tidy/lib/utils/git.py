#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

import sys
import yaml
from pathlib import Path

from git import Repo, InvalidGitRepositoryError


def get_repo_info(path: str):
    """
    Get information about the repository at the given path.
    Returns data as a dictionary.
    """

    path: Path = Path(path)
    try:
        # Initialize the repository object
        repo = Repo(path.absolute(), search_parent_directories=True)

        # Get the top-level directory of the repository
        top_level_dir = Path(repo.git.rev_parse("--show-toplevel"))

        # Get the repository name (directory name of the top level)
        repo_name = top_level_dir.name

        # Get the remote URL (assuming 'origin' is the remote name)
        remote_url = None
        if "origin" in repo.remotes:
            remote_url = repo.remotes.origin.url

        # Get the current branch
        current_branch = repo.active_branch.name

        # Get the commit hash
        commit_hash = repo.head.commit.hexsha

        # Get the status of the repository
        status = repo.git.status()

        # Get unstaged changes
        unstaged_changes = repo.git.diff("--stat")

        return {
            "top_level": top_level_dir,
            "name": repo_name,
            "remote_url": remote_url,
            "current_branch": current_branch,
            "commit_hash": commit_hash,
            "status": status,
            "unstaged_changes": unstaged_changes,
        }
    except InvalidGitRepositoryError:
        print(f"The path '{path}' is not inside a valid Git repository.")
        sys.exit(1)
    except Exception:
        print(f"An error occurred while getting repository information. Check {path}.")
        sys.exit(1)


def get_beman_standard_config_path():
    """
    Get the path to the Beman Standard YAML configuration file.
    """
    return Path(__file__).parent.parent.parent / ".beman-standard.yml"


def load_beman_standard_config(path=get_beman_standard_config_path()):
    """
    Load the Beman Standard YAML configuration file from the given path.
    """
    with open(path, "r") as file:
        beman_standard_yml = yaml.safe_load(file)

    beman_standard_check_config = {}
    for check_name in beman_standard_yml:
        check_config = {
            "name": check_name,
            "full_text_body": "",
            "type": "",
            "regex": "",
            "file_name": "",
            "directory_name": "",
            "badge_lines": "",
            "status_lines": "",
            "licenses": "",
            "default_group": "",
        }
        for entry in beman_standard_yml[check_name]:
            if "type" in entry:
                check_config["type"] = entry["type"]
            elif "value" in entry:  # e.g., "a string value"
                check_config["value"] = entry["value"]
            # e.g., ["a string value", "another string value"]
            elif "values" in entry:
                check_config["values"] = entry["values"]
            elif "regex" in entry:
                # TODO: Implement the regex check.
                pass
            elif "file_name" in entry:
                check_config["file_name"] = entry["file_name"]
            elif "directory_name" in entry:
                pass
            elif "values" in entry:
                # TODO: Implement the values check.
                pass
            elif "status_lines" in entry:
                # TODO: Implement the status check.
                pass
            elif "licenses" in entry:
                # TODO: Implement the license check.
                pass
            elif "default_group" in entry:
                check_config["default_group"] = entry["default_group"]
            else:
                raise ValueError(f"Invalid entry in Beman Standard YAML: {entry}")

        beman_standard_check_config[check_name] = check_config

    return beman_standard_check_config
