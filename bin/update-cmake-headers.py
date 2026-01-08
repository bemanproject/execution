#!/usr/bin/python3
# bin/update-cmake-headers.py
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

import glob
import os
import re
import sys

head_re = re.compile("include/(?P<name>.*)\.hpp")


def clean_name(file):
    match = head_re.match(file)
    return match.group("name")


def get_headers(dir):
    result = []
    for header in glob.glob(dir + "/*.hpp"):
        result.append(clean_name(header))
    result.sort()
    return result


file_set_re = re.compile(" *FILE_SET.*")
section_re = re.compile(" *FILE_SET *\${TARGET_NAME}_(?P<section>.*)_headers$")
header_re = re.compile(" *\${PROJECT_SOURCE_DIR}/include/beman/.*/.*\.hpp")

if len(sys.argv) != 2:
    print(f"usage: {sys.argv[0]} <path>/CMakeLists.txt")
    sys.exit(1)

cmake = sys.argv[1]
print(f"updating {cmake}")

section = ""
section_done = False
sections = {
    "public": get_headers("include/beman/*"),
    "detail": get_headers("include/beman/*/detail"),
}

with open(cmake, "r") as input:
    lines = input.readlines()

with open(f"{cmake}", "w") as output:
    for line in lines:
        if None != file_set_re.match(line):
            section = ""
        match = section_re.match(line)
        if None != match:
            section = match.group("section")
            section_done = False
        if None != header_re.match(line):
            if not section_done:
                section_done = True
                project = "${PROJECT_SOURCE_DIR}"
                for header in sections[section]:
                    output.write(f"                {project}/include/{header}.hpp\n")
        else:
            output.write(line)
            pass
