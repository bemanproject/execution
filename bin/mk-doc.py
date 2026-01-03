#!/usr/bin/python3

import re
import sys

prestart_re = re.compile('\s*<pre name="([^\"]*)">')
preend_re = re.compile("\s*</pre>")
append_re = re.compile("list\(APPEND EXAMPLES")
appendp_re = re.compile("list\(APPEND EXAMPLES\)")
paren_re = re.compile("\)")

def update_cmake(list):
    text = ""
    skipping = False
    with open("docs/code/CMakeLists.txt") as cmake:
        for line in cmake:
            if skipping:
                if paren_re.match(line):
                    skipping = False
                    text += line
            else:
                if append_re.match(line):
                    text += "list(APPEND EXAMPLES\n"
                    text += "\n".join(list) + "\n"
                    skipping = not appendp_re.match(line)
                    if not skipping:
                        text += ")\n"
                else:
                    text += line

    with open("docs/code/CMakeLists.txt", "w") as cmake:
        cmake.write(text)


def transform(text, output):
    capturing = False
    code = ""
    name = ""
    names = {}
    for line in text:
        match = prestart_re.match(line)
        if match:
            print(f"example: {match.group(1)}")
            name = match.group(1)
            code = ""
            output.write(f"```c++\n")
            capturing = True
        elif capturing and preend_re.match(line):
            if name in names:
                print(f"skipping code with duplicate name '{name}'")
            else:
                names[name] = code
                with open("docs/code/" + name + ".cpp", "w") as codeout:
                    codeout.write(code)
            capturing = False
            output.write(f"```\n")
        else:
            if capturing:
                code += line
            output.write(f"{line}")
    update_cmake(names.keys())


def process(root):
    print(f"processing {root}")
    text = []
    with open(root + ".mds") as input:
        text = input.readlines()
    with open(root + ".md", "w") as output:
        transform(text, output)

sys.argv.pop(0)
for file in sys.argv:
    match = re.match("(.*)\.mds$", file)
    if match:
        process(match.group(1))
