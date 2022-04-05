#!/usr/bin/python3

from ast import Mult
from itertools import chain, combinations, product
from os import path
from typing import List

snippet_class = "public class <TYPE>_<NAME><ID>{\n<SNIPPET>\n}"
snippet_main = snippet_class.replace(
    "<SNIPPET>", "public static void main(String[] args){\n<SNIPPET>\n}")


class Token:
    def __init__(self, elements) -> None:
        self.elements = elements

    def compile(self, passing: bool) -> List[str]:
        return [""]


class Any(Token):
    def __init__(self, elements) -> None:
        if isinstance(elements, str):
            super().__init__([elements])
        else:
            super().__init__(elements)

    def compile(self, passing: bool) -> List[str]:
        opts = []

        if not self.elements:
            return opts

        if isinstance(self.elements, Token):
            return self.elements.compile(passing)

        for element in self.elements:
            if isinstance(element, str):
                opts.append(element)
            elif isinstance(element, Token):
                opts.extend(element.compile(passing))

        return opts


class Optional(Token):
    def __init__(self, elements, affects_fail=False) -> None:
        if isinstance(elements, list):
            super().__init__(Any(elements))
        elif isinstance(elements, str):
            super().__init__(Any([elements]))
        else:
            super().__init__(elements)
        self.affects_fail = affects_fail

    def compile(self, passing: bool) -> List[str]:
        if not (passing or self.affects_fail or self.elements):
            return []

        if passing:
            return self.elements.compile(passing) + [""]
        if self.affects_fail:
            return self.elements.compile(passing)


class Wrap(Token):
    def __init__(self, elements, prefix="", suffix="") -> None:
        if isinstance(elements, list):
            super().__init__(Any(elements))
        else:
            super().__init__(elements)
        self.prefix = prefix
        self.suffix = suffix

    def format(self, string):
        return f"{self.prefix}{string}{self.suffix}"

    def compile(self, passing: bool) -> List[str]:
        if not self.elements:
            return []
        elif isinstance(self.elements, str):
            return [self.format(self.elements)]
        elif isinstance(self.elements, Token):
            return map(self.format, self.elements.compile(passing))


class Group(Token):
    def __init__(self, elements) -> None:
        super().__init__(elements)

    def get_token_choices(self, passing: bool) -> List[str]:
        choices = []
        for element in self.elements:
            if isinstance(element, str):
                choices.append([element])
            elif isinstance(element, Token):
                choices.append(element.compile(passing))
            elif isinstance(element, list):
                choices.append(Any(element).compile(passing))
        return choices

    def compile(self, passing: bool) -> List[str]:
        if not self.elements:
            return []
        choices = self.get_token_choices(passing)
        choices = filter(None, choices)
        choices = product(*choices)
        if not passing:
            choices = map(lambda x: combinations(x, len(x) - 1), choices)
            choices = chain(*choices)
        choices = map(' '.join, choices)
        return sorted(set(choices))


class Repeat(Group):
    def __init__(self, elements, count, separator=",") -> None:
        if isinstance(elements, list):
            super().__init__(Any(elements))
        elif isinstance(elements, str):
            super().__init__(Any([elements]))
        else:
            super().__init__(elements)
        self.count = count
        self.separator = separator

    def get_token_choices(self, passing: bool) -> List[str]:
        choices = [self.elements.compile(passing) for i in range(self.count)]
        for i in range(self.count - 1):
            choices.insert(2*i + 1, self.separator)
        return choices

    def compile(self, passing: bool) -> List[str]:
        if not self.elements:
            return []
        if self.count == 0:
            return []
        if self.count == 1:
            return self.elements.compile(passing)
        return super().compile(passing)


class Count(Group):
    def __init__(self, elements, count, separator=",") -> None:
        if isinstance(elements, list):
            super().__init__(Any(elements))
        elif isinstance(elements, str):
            super().__init__(Any([elements]))
        else:
            super().__init__(elements)
        self.range = count if isinstance(count, range) else range(count)
        self.separator = separator

    def compile(self, passing: bool) -> List[str]:
        if not self.elements:
            return []
        opts = []
        for n in self.range:
            repeat = Repeat(self.elements, n + 1, self.separator)
            opts.extend(repeat.compile(passing))
        return sorted(set(opts))


class Switch(Token):
    def __init__(self, passing=None, fail=None) -> None:
        self.passing = Any(passing)
        self.fail = Any(fail)

    def compile(self, passing: bool) -> List[str]:
        if passing and self.passing:
            return self.passing.compile(True)
        elif not passing and self.fail:
            return self.fail.compile(True)
        return []


class Join(Token):
    def __init__(self, elements, separator=",") -> None:
        if isinstance(elements, list):
            super().__init__(Any(elements))
        elif isinstance(elements, str):
            super().__init__(Any([elements]))
        else:
            super().__init__(elements)
        self.separator = separator

    def compile(self, passing: bool) -> List[str]:
        return [self.separator.join(self.elements.compile(passing))]


class_prefix, class_suffix = "public class <TYPE>_<NAME><ID> {", "}"
main_prefix = class_prefix + "public static void main(String[] args){"
main_suffix = class_suffix + "}"

singles = Any(["int", "String"])
arrays = Wrap(singles, suffix="[]")
types = Any([singles, arrays])
var = Group([types, Any(["name"])])
exp = Any(["true", "true || false"])
dummy = Optional(['System.out.println("rQqOksxPcXb");'])
args = Any(["int i", "String s"])
literals = Any(["3", "'c'"])


def println(elements):
    return Wrap(elements, "System.out.println(", ");")


def ops2(left, right, ops):
    cases = []
    for op in ops:
        cases.append(Join(println(Group([left, op, right])), "\n"))
    return Any(cases)


patterns = {
    "syn": {
        "class": Group(["public",
                        "class",
                        "<TYPE>_<NAME><ID>",
                        "{",
                        "}"]),
        "class_var": Wrap(Group(["public",
                                 "static",
                                 types,
                                 "name",
                                 Optional(["= 3", '= "cmgYhq"']),
                                 ";"]),
                          prefix=class_prefix,
                          suffix=class_suffix),
        "method": Wrap(Group(["public",
                              "static",
                              ["void", types],
                              "main",
                              "(",
                              Optional(Count(args, 2)),
                              ")",
                              "{",
                              dummy,
                              "}"]),
                       prefix=class_prefix,
                       suffix="}"),
        "for": Wrap(Group(["for",
                           "(",
                           Optional(["int i = 0",
                                     "int i"]),
                           ";",
                           Optional(["true",
                                     "args.length"]),
                           ";",
                           Optional("i++"),
                           ")",
                           "{",
                           dummy,
                           "}"]),
                    prefix=main_prefix,
                    suffix=main_suffix),
        "if": Wrap(Group(["if",
                          "(",
                          exp,
                          ")",
                          "{",
                          dummy,
                          "}"]),
                   prefix=main_prefix,
                   suffix=main_suffix),
        "switch": Wrap(Group(["switch",
                              "(",
                              "args.length",
                              ")",
                              "{",
                              Optional(["case 1: break;",
                                        "default: break;"]),
                              "}"]),
                       prefix=main_prefix,
                       suffix=main_suffix),
        "cases": Wrap(Group([["default", Group(["case", ["53"]])],
                             ":",
                             ["break;", "return;", "return 32;"]]),
                      prefix=main_prefix+"switch(args.length){",
                      suffix=main_suffix+"}"),
        "while": Wrap(Group(["while",
                             "(",
                             exp,
                             ")",
                             "{",
                             dummy,
                             "}"]),
                      prefix=main_prefix,
                      suffix=main_suffix),
        "call": Wrap([Group(["method",
                            "(",
                             Optional(Count(literals, 2)),
                             ")",
                             ";"])],
                     prefix=main_prefix,
                     suffix=main_suffix)
    },
    "sem": {
        "assign": Wrap(Switch([Group([["int", "double", "char"],
                                      "value",
                                      Optional("; value"),
                                      "= 82;"]),
                               Group(["String value",
                                      Optional("; value"),
                                      '= "TeaWvaM";'])],
                              [Group([["int", "char"],
                                      "value",
                                     Optional("; value"),
                                     '= "HhkOjtJMn";']),
                               Group(["String value",
                                      Optional("; value"),
                                      "=",
                                      ["26", "'L'"],
                                      ";"])]),
                       prefix=main_prefix,
                       suffix=main_suffix),
        "array": Wrap(Switch([Group(["int[] values = new int[",
                                     ["3", "0", "99"],
                                     "];"]),
                              Group(["String[] values = new String[",
                                     ["3", "0", "99"],
                                     "];"])],
                             [Group(["int[] values;\nint i = values[",
                                     ['"UyaMcVU"', "true"],
                                     "];"]),
                              Group(["String[] values;\nString i = values[",
                                     ['"UyaMcVU"', "true"],
                                     "];"])]),
                      prefix=main_prefix,
                      suffix=main_suffix),
        "call": Wrap(Switch(Group([["4", "i"],
                                   ",",
                                   ['"AyHeqD"', "s"]]),
                            [Group([["3", "i"],
                                    ",",
                                    ["10", "i", "true"],
                                    Optional([", 7", ", i", ", s"])]),
                             Group([['"uDhviJapoIQphKtyLO"', "s"],
                                    ",",
                                    ['"ZglfOdUGBOn"', "s"]])]),
                     prefix=class_prefix +
                     "public static int method(int i, String s){}\npublic static void main(String[] args){\nint i; String s;\nmethod(",
                     suffix=");"+main_suffix),
        "ops": Wrap(Switch([ops2(["14", "'f'", "43.51d", '"tsLRlkxnQtkwmmQUwFLU"',
                                 "i", "d", "c", "s"],
                                 ["24", "'p'", "74.1d", '"aXCwxJWglaSaCqKR"',
                                 "i", "d", "c", "s"],
                                 ["+"]),
                            ops2(["14", "'f'", "43.51d", "i", "d", "c"],
                                 ["24", "'p'", "74.1d", "i", "d", "c"],
                                 ["-", "*", "/", "%", "<", "<=", ">", ">="]),
                            ops2(["true", "false"],
                                 ["true", "false"],
                                 ["||", "&&", "==", "!="]),
                            ops2(["14", "'f'", "43.51d", "i", "d", "c"],
                                 ["24", "'p'", "74.1d",  "i", "d", "c"],
                                 ["==", "!="]),
                            ops2(['"SMiqkH"', "s", "null"],
                                 ['"NxkbE"', "s", "null"],
                                 ["==", "!="]),
                            ops2(["ints", "null"],
                                 ["ints", "null"],
                                 ["==", "!="]),
                            ops2([""], ["1", "'h'", "5.30d", "i", "c", "d"], ["-"]),
                            ops2(["i", "c", "d"], [""], ["++", "--"]),
                            ops2([""], ["true", "false"], ["!"])]),
                    prefix=main_prefix +
                    'int i = 64;\nchar c = \'O\';\ndouble d = 71.87d;\nString s = "CjxDkIN";\nint[] ints = null;\n\n',
                    suffix=main_suffix)
    }
}


for pattern_type, patterns in patterns.items():
    for name, pattern in patterns.items():
        for passing, directory in zip([True, False], ["j0", "errors"]):
            for i, case in enumerate(pattern.compile(passing)):
                case = case.replace("<TYPE>", pattern_type)
                case = case.replace("<NAME>", name)
                case = case.replace("<ID>", str(i + 1))
                case = case.replace("{", "{\n").replace("}", "\n}")
                filename = path.join("tests", directory,
                                     f"{pattern_type}_{name}{i+1}.java")
                with open(filename, 'w') as f:
                    f.write(case)
