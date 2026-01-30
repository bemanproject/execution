#!/usr/bin/python3
# bin/mk-module.py
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

import glob
import inspect
import os
import re
import sys

head_re = re.compile("include/(?P<name>.*)\.hpp")


def make_loc(file, number):
    loc = {}
    loc["number"] = number
    loc["file"] = file
    return loc


class common_writer:
    def write(self, loc, line):
        self.do_write(loc, line)
    def write_same(self, line):
        self.do_write(self.loc, line)
    def write_next(self, line):
        self.do_next()
        self.do_write(self.do_get_loc(), line)


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

class comment_writer(common_writer):
    start_re = re.compile("(.*)/\*.*")
    end_re = re.compile(".*\*/\s*(.*)")
    def __init__(self, to):
        self.to = to
        self.in_comment = False

    def do_get_loc(self):
        return self.to.do_get_loc()
    def do_next(self):
        self.to.do_next()
    def do_write(self, loc, line):
        if self.in_comment:
            match = self.end_re.match(line)
            if match:
                self.in_comment = False
                self.to.write(loc, match.group(1))
        else:
            match = self.start_re.match(line)
            if match:
                self.in_comment = True
                line = match.group(1).rstrip()
            self.to.write(loc, line)

class filter_writer(common_writer):
    included_re = re.compile(".*INCLUDED_BEMAN.*")
    file_re = re.compile("// include/beman\S*\s*-.-C..-.-")
    spdx_re = re.compile(".*SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception.*")
    namespace_re = re.compile(".*// namespace.*")
    comment_re = re.compile("(.*)//.*")
    close_re = re.compile("^\s*}\s*$")
    public_re = re.compile("^\s*public:\s*$")
    else_re = re.compile("^\s*#\s*else\s*$")

    def __init__(self, to):
        self.to = to
        self.previous_line_empty = False

    def do_get_loc(self):
        return self.to.do_get_loc()
    def do_next(self):
        self.to.do_next()

    def include_this_line(self, line):
        return (
            not beman_re.match(line)
            and not other_re.match(line)
            and not self.included_re.match(line)
            and not self.file_re.match(line)
            and not self.spdx_re.match(line)
        )

    def do_write(self, loc, line):
        if not self.include_this_line(line):
            return
        match = self.comment_re.match(line)
        if match and not self.namespace_re.match(line):
            line = match.group(1).rstrip()
        if line != "" or not self.previous_line_empty:
            self.previous_line_empty = (
                (line == "")
                or (self.close_re.match(line) is not None)
                or (self.public_re.match(line) is not None)
                or (self.else_re.match(line) is not None)
            )
            self.to.write(loc, line)


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
export_re = re.compile("BEMAN_EXECUTION_EXPORT (.*)")


def write_header(to, header):
    filename = f"include/{header}.hpp"
    with open(filename) as file:
        number = 0
        for line in file.readlines():
            number += 1
            match = export_re.match(line)
            loc = make_loc(filename, number)
            if match:
                to.write(loc, f"export {match.group(1)}")
            else:
                to.write(loc, line.rstrip())


deps = {}


def build_header(file, header):
    todo = dependencies[header].copy()
    while 0 < len(todo):
        if not todo[0] in deps:
            deps[todo[0]] = dependencies[todo[0]].copy()
            for new in dependencies[todo[0]]:
                todo.append(new)
        todo = todo[1:]

class file_writer(common_writer):
    def __init__(self, to):
        self.to = to
        self.loc = make_loc("", 0)

    def do_get_loc(self):
        return self.loc
    def do_next(self):
        self.loc["number"] += 1
    def do_write(self, loc, line):
        #self.to.write(f"loc={loc} self.loc={self.loc}")
        if loc["file"] != self.loc["file"] or loc["number"] != self.loc["number"]:
            self.to.write(f"#line {loc['number']} \"{loc['file']}\"\n")
        self.to.write(f"{line}\n")
        self.loc = loc
        self.loc["number"] += 1


with open(module_file, "w") as file:
    #file = sys.stdout
    file_to = file_writer(file)
    to = filter_writer(file_to)
    to = comment_writer(to)

    file_to.write(make_loc(sys.argv[0], inspect.currentframe().f_lineno), "// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception")
    file_to.write_next("// *****************************************************;")
    file_to.write_next("// *** WARNING: this file is generated: do not edit! ***;")
    file_to.write_next("// *****************************************************;")
    file_to.write_next(f"// generated by {sys.argv[0]} {sys.argv[1]}")
    file_to.write_next("")
    file_to.write_next("module;")
    file_to.write_next("")
    file_to.write_next("#include <beman/execution/modules_export.hpp>")
    file_to.write_next("#include <cassert>")
    file_to.write_next("")
    file_to.write_next("#ifdef BEMAN_HAS_IMPORT_STD")
    file_to.write_next("import std;")
    file_to.write_next("#else")
    includes = list(headers.keys())
    for include in sorted(includes):
        file_to.write(make_loc(sys.argv[0], inspect.currentframe().f_lineno), f"#include <{include}>")
    file_to.write_next("#endif")
    file_to.write_next("")
    file_to.write_next("export module beman.execution;")
    file_to.write_next("")

    for header in top:
        if re.match(".*execution26.*", header):
            continue

        prolog_done = False
        filename = f"include/{header}.hpp"
        with open(filename) as file:
            number = 0
            for line in file.readlines():
                number += 1
                if not prolog_done and define_re.match(line):
                    prolog_done = True
                    to.write(make_loc(filename, number), "")
                    build_header(file, header)
                    to.write_next("")

    while 0 < len(deps):
        empty = [item for item in deps.keys() if 0 == len(deps[item])]
        for e in empty:
            write_header(to, e)
            deps.pop(e, None)
            for d in deps.keys():
                deps[d] = [item for item in deps[d] if e != item]
