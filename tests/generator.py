#!/usr/bin/python3

from ast import Mult
from itertools import chain, combinations, product
from os import path
from typing import Iterable, List

snippet_class = "public class <TYPE>_<NAME><ID>{\n<SNIPPET>\n}"
snippet_main = snippet_class.replace(
    "<SNIPPET>", "public static void main(String[] args){\n<SNIPPET>\n}")


class Token:
    def __init__(self, elements) -> None:
        if isinstance(elements, Iterable):
            self.elements = Any(elements)
        elif isinstance(elements, str):
            self.elements = Any([elements])
        else:
            self.elements = elements

    def compile(self, passing: bool) -> List[str]:
        return [""]


class Any(Token):
    def __init__(self, elements) -> None:
        if isinstance(elements, str):
            self.elements = [elements]
        else:
            self.elements = elements

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
        if isinstance(elements, str):
            self.elements = [elements]
        elif isinstance(elements, Iterable):
            self.elements = elements
        else:
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
        super().__init__(Any(elements))
        self.count = count
        self.separator = separator

    def get_token_choices(self, passing: bool) -> List[str]:
        choices = []
        for i in range(self.count):
            choice = self.elements.compile(passing)
            if i > 0:
                if isinstance(self.separator, str):
                    choices.append([self.separator])
                elif isinstance(self.separator, Iterable):
                    choices.append(list(self.separator))
            if isinstance(choice, str):
                choices.append([choice])
            else:
                choices.append(choice)
        return choices

    def compile(self, passing: bool) -> List[str]:
        if not self.elements or self.count <= 0:
            return []
        if self.count == 1:
            return self.elements.compile(passing)
        return super().compile(passing)


class Count(Group):
    def __init__(self, elements, count, separator=",") -> None:
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
        super().__init__(elements)
        self.separator = separator

    def compile(self, passing: bool) -> List[str]:
        return [self.separator.join(self.elements.compile(passing))]


class Replace(Token):
    def __init__(self, elements, old, new) -> None:
        super().__init__(elements)
        if isinstance(old, str):
            self.old = old
        elif isinstance(old, list):
            self.old = old[0]
            if len(old) > 1:
                self.elements = Replace(elements, old[1:], new)

        self.new = new if isinstance(new, list) else [new]

    def compile(self, passing: bool) -> List[str]:
        if not self.old or not self.new:
            return []
        for case in self.elements.compile(passing):
            for new in self.new:
                yield case.replace(self.old, new)


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
    return Replace(Join(println(Group([left, "<OP>", right])), "\n"), "<OP>", ops)


patterns = {
    "syn": {
        "class": Group(["public",
                        "class",
                        "<TYPE>_<NAME><ID>",
                        "{",
                        Optional("public static int i;\n"),
                        Optional("public static void main(String[]args){}\n"),
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
                                     "i < 10"]),
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
        "if_else": Wrap(Group(['if (true){ System.out.println("FMgRI"); }',
                               "else {",
                               dummy,
                               "}"]),
                        prefix=main_prefix,
                        suffix=main_suffix),
        "if_if": Wrap(Count(Group(["else if (true) {",
                                   dummy,
                                   "}"]),
                            range(1, 2), ""),
                      prefix=main_prefix +
                      'if (true){ System.out.println("QDwVICTpGWCdfgKo"); }',
                      suffix=main_suffix),
        "if_if_else": Wrap(Group(["else {",
                                  dummy,
                                  "}"]),
                           prefix=main_prefix +
                           'if (true){ System.out.println("QDwVICTpGWCdfgKo"); } else if(true){ System.out.println("uxrNIuLEXeuYBghnJ");}',
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
                             Optional(["break;", "continue;"]),
                             "}"]),
                      prefix=main_prefix,
                      suffix=main_suffix),
        "call": Wrap([Group(["method",
                            "(",
                             Optional(Count(literals, 2)),
                             ")",
                             ";"])],
                     prefix=main_prefix,
                     suffix=main_suffix),
        "statement": Wrap([Switch(["return;",
                                   "return 32;",
                                   "break;",
                                   "continue;"],
                                  ["return",
                                   "return 32",
                                   "break",
                                   "continue"]),
                           Group(["a", ["++", "--"], ";"]),
                           Group(["method", "()", ";"]),
                           Group(["a", "=", "41", ";"]),
                           Group(["a", "=", "new", ["int", "String"], "[", "32", "]", ";"])],
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
                                      '= "TeaWvaM";']),
                               "int[] ints;\nints[0] = 3;"],
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
        "access": Wrap(Switch(["int[] ints; int i = ints[0];",
                               "String[] Strings; String s = Strings[0];"]),
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
                     "public static int method(int i, String s){return 33;}\npublic static void main(String[] args){\nint i; String s;\nmethod(",
                     suffix=");"+main_suffix),
        "if": Wrap(Switch(None,
                          ["if(3){}",
                           "if(false){}else if(2){}",
                           "if(false){}else if(false){}else if(3){}"]),
                   prefix=main_prefix,
                   suffix=main_suffix),
        "for": Wrap(Switch(["return;", "break;", "continue;"],
                           ["return 61;",
                           Group(["return;", "int a;"])]),
                    prefix=main_prefix + "for(int i=0;i<10;i++){",
                    suffix="}" + main_suffix),
        "while": Wrap(Switch(["return;", "break;", "continue;"],
                             ["return 61;",
                              Group(["return;", "int a;"])]),
                      prefix=main_prefix + "while(true){",
                      suffix="}" + main_suffix),
        "op": Wrap(Switch([ops2(["14", "'f'", "43.51d", '"tsLRlkxnQtkwmmQUwFLU"',
                                 "i", "d", "c", "s"],
                                ["14", "'f'", "43.51d", '"aXCwxJWglaSaCqKR"',
                                 "i", "d", "c", "s"],
                                ["+"]),
                           ops2(["14", "'f'", "43.51d", "i", "d", "c"],
                                ["14", "'p'", "43.51d", "i", "d", "c"],
                                ["-", "*", "/", "%", "<", "<=", ">", ">="]),
                           ops2(["true", "false"],
                                ["true", "false"],
                                ["||", "&&", "==", "!="]),
                           ops2(["14", "'f'", "43.51d", "i", "d", "c"],
                                ["14", "'p'", "43.51d",  "i", "d", "c"],
                                ["==", "!="]),
                           ops2(['"SMiqkH"', "s", "null"],
                                ['"NxkbE"', "s", "null"],
                                ["==", "!="]),
                           ops2(["ints", "null"],
                                ["ints", "null"],
                                ["==", "!="]),
                           ops2([""], ["1", "'h'", "5.30d", "i", "c", "d"], ["-"]),
                           ops2(["i", "c", "d"], [""], ["++", "--"]),
                           ops2([""], ["true", "false"], ["!"])],
                          [println(Group([["i", "4", "d", "59.10d", "c", "'k'"],
                                          ["+", "-", "*", "/", "%", "||", "&&",
                                           "<", "<=", ">", ">=", "==", "!="],
                                          ["true"]])),
                           println(Group([["i", "5", "d", "41.37d", "c", "'z'"],
                                          ["<", "<=", ">", ">=",
                                              "==", "!=", "&&", "||"],
                                          ["s"]])),
                           println(Group([["s", '"VsSUsAByDmshAOnlE"'],
                                          ["<", "<=", ">", ">=",
                                              "==", "!=", "&&", "||"],
                                          ["true"]])),
                           println(Group([["ints"],
                                          ["+", "-", "*", "/", "%", "||", "&&",
                                           "<", "<=", ">", ">=", "==", "!="],
                                          ["i", "85", "d", "c", "s", "true"]]))
                           ]),
                   prefix=main_prefix +
                   'int i = 64;\nchar c = \'O\';\ndouble d = 71.87d;\nString s = "CjxDkIN";\nint[] ints = null;\n\n',
                   suffix=main_suffix),
        "statement": Wrap(Switch(None,
                                 ["return 60;",
                                  "continue;",
                                  "break;"]),
                          prefix=main_prefix,
                          suffix=main_suffix),
        "switch": Wrap(Switch(Group([Optional("case 1: return;\n"),
                                     Optional("case 3: break;\n"),
                                     Optional("case 4: return;\n"),
                                     Optional("default: break;\n")]),
                              "case 1: break;\ncase 1: return;"),
                       prefix=main_prefix + "int i = 4; switch(i){\n",
                       suffix="\n}" + main_suffix)
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
                    f.write("// Generated with generator.py\n")
                    f.write(case)
