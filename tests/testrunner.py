#!/bin/python3

import os
from os import path
from subprocess import Popen, PIPE
from sys import stdout

# Path to J0 executable
j0 = "./j0"
# Only print failed tests instead of all
only_failed = False

test_dir = "tests"

LEX = 1
SYNTAX = 2
SEMATIC = 3
PASS = 4
FAIL = 5

passed_tests = []
failed_tests = []

os.system("make")


def generate_lex(input_file, out_dir):
    with open(input_file, 'r') as words:
        for i, word in enumerate(words.read().split()):
            with open(f"tests/{out_dir}/lex{i}.java", "w") as test:
                test.write(word)


generate_lex(path.join(test_dir, "lex_legal.txt"), "j0")
generate_lex(path.join(test_dir, "lex_illegal.txt"), "errors")
generate_lex(path.join(test_dir, "lex_java.txt"), "java")


def test_file(outcome, level, file):
    p = Popen(args=[j0, file], stderr=PIPE, stdout=PIPE)

    std_out, std_err = p.communicate()
    std_out = std_out.decode("utf-8")
    std_err = std_err.decode("utf-8")

    passed = False

    if outcome == PASS:
        if level == LEX and p.returncode != LEX:
            passed = True
        elif level == SYNTAX and (p.returncode == 0 or p.returncode > SYNTAX):
            passed = True
        elif level == SEMATIC and p.returncode == 0:
            passed = True
        else:
            passed = False
    else:
        passed = p.returncode == level

    if passed:
        passed_tests.append(f"PASS {file}")
    else:
        failed_tests.append(
            f"FAIL {file} exit code: {p.returncode}\n{std_err}")


def test_directory(result, directory):
    for root, dirs, files in os.walk(directory):
        for filename in sorted(files):
            if filename.endswith(".java"):
                file = path.join(directory, filename)

                if filename.startswith("lex"):
                    test_file(result, LEX, file)
                elif filename.startswith("syn"):
                    test_file(result, SYNTAX, file)
                elif filename.startswith("sem"):
                    test_file(result, SEMATIC, file)


test_directory(FAIL, path.join(test_dir, "java"))
test_directory(FAIL, path.join(test_dir, "errors"))
test_directory(PASS, path.join(test_dir, "j0"))

if len(failed_tests) == 0 or not only_failed:
    for result in passed_tests:
        print(result)
if len(failed_tests) != 0 or not only_failed:
    for result in failed_tests:
        print(result)
