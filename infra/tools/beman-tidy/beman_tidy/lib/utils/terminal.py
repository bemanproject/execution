#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

import subprocess


def run_command(command, return_stdout=False, cwd=None):
    """
    Run a command in the shell and return the return code.
    If return_stdout is True, return the stdout of the command.
    Optionally, change the current working directory to cwd.
    """
    print(f"Running command: {command} with cwd: {cwd}")
    if return_stdout:
        bin = subprocess.Popen(
            command, shell=True, stdout=subprocess.PIPE, cwd=cwd
        ).stdout.read()
        return bin.decode("utf-8")
    else:
        return subprocess.run(command, shell=True, cwd=cwd).returncode
