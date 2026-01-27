#!/usr/bin/python3
# bin/mk-module.py
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

import glob
import os
import re
import sys

head_re = re.compile("include/(?P<name>.*)\.hpp")


def clean_name(file):
    match = head_re.match(file)
    return match.group("name")


top = []
for toplevel in glob.glob("include/?eman/*/*.hpp"):
    top.append(clean_name(toplevel))

all = top.copy()
for detail in glob.glob("include/?eman/*/?etail/*.hpp"):
    all.append(clean_name(detail))

headers = {}
beman_re = re.compile('#include ["<](?P<name>[bB]eman/.*)\.hpp[">]')
other_re = re.compile('#include ["<](?P<name>.*)[">]')
included_re = re.compile('.*INCLUDED_BEMAN.*')
file_re = re.compile('// include/beman\S*\s*-.-C..-.-')
spdx_re = re.compile('.*SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception.*')



def include_this_line(line):
    return not beman_re.match(line) and not other_re.match(line) and not included_re.match(line) and not file_re.match(line) and not spdx_re.match(line)


def get_dependencies(component):
    deps = []
    with open("include/" + component + ".hpp") as file:
        for line in file.readlines():
            if beman_re.match(line):
                deps.append(beman_re.match(line).group("name"))
            elif other_re.match(line):
                header = other_re.match(line).group("name")
                if header not in headers:
                    headers[header] = 1

    return deps


dependencies = {}

for component in all:
    dependencies[component] = get_dependencies(component)

if len(sys.argv) != 2:
    print(f"usage: {sys.argv[0]} <module-file>")
    sys.exit(1)

module_file = sys.argv[1]

project_re = re.compile("(?P<project>(?P<beman>[bB]eman)/.*)/")
define_re = re.compile("#define")


def write_header(to, header):
    with open(f"include/{header}.hpp") as file:
        for line in file.readlines():
            if include_this_line(line):
                to.write(line)


deps = {}
def build_header(file, to, header):
    todo = dependencies[header].copy()
    while 0 < len(todo):
        if not todo[0] in deps:
            deps[todo[0]] = dependencies[todo[0]].copy()
            for new in dependencies[todo[0]]:
                todo.append(new)
        todo = todo[1:]

with open(module_file, "w") as to:
    to.write("// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception\n\n")
    to.write("#ifdef USE_STD_MODULE\n")
    to.write("import std;\n")
    to.write("#else\n")
    includes = list(headers.keys())
    for include in sorted(includes):
        to.write(f"#include <{include}>\n")
    to.write("#endif\n\n")

    to.write("module beman.execution;\n\n")

    for header in top:
        if re.match(".*execution26.*", header):
            continue
        print(f"Building module for {header}")

        prolog_done = False
        with open(f"include/{header}.hpp") as file:
            for line in file.readlines():
                if not prolog_done and define_re.match(line):
                    prolog_done = True
                    to.write("\n")
                    build_header(file, to, header)
                    to.write("\n")

    while 0 < len(deps):
        empty = [item for item in deps.keys() if 0 == len(deps[item])]
        for e in empty:
            write_header(to, e)
            deps.pop(e, None)
            for d in deps.keys():
                deps[d] = [item for item in deps[d] if e != item]
