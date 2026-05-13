#!/usr/bin/env python3
"""Lists plat-lang source words extracted from implementation files."""

from __future__ import annotations

import argparse
import json
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]

LEXER_TOKENS = ROOT / "src" / "platlang_lexer.l"
INTERPRETER = ROOT / "src" / "interpreter.cpp"
CORE_BUILTINS = ROOT / "src" / "builtins" / "core_builtins.cpp"
CANVAS_BUILTINS = ROOT / "src" / "builtins" / "canvas_builtins.cpp"

CONSTANT_TOKENS = {"NIKS", "WOAR", "KWATSJ"}
OPERATOR_TOKENS = {"AND", "OR"}
TYPE_WORDS = {"nommer", "nómmer", "nómmer", "teks", "tabel"}


def decode_cpp_string(value: str) -> str:
    """Decodes the C++ string escapes used by keyword constants."""
    raw = bytearray()
    i = 0
    while i < len(value):
        if value[i] != "\\":
            raw.extend(value[i].encode("utf-8"))
            i += 1
            continue

        i += 1
        if i >= len(value):
            raw.append(ord("\\"))
            break

        escape = value[i]
        if escape == "x" and i + 2 < len(value):
            raw.append(int(value[i + 1 : i + 3], 16))
            i += 3
            continue

        translations = {
            "n": "\n",
            "r": "\r",
            "t": "\t",
            "\\": "\\",
            '"': '"',
        }
        raw.extend(translations.get(escape, escape).encode("utf-8"))
        i += 1

    return raw.decode("utf-8")


def unique_sorted(words: set[str]) -> list[str]:
    """Returns words once, sorted by normalized spelling."""
    return sorted(words, key=lambda word: (word.casefold(), word))


def read(path: Path) -> str:
    """Reads a repository text file as UTF-8."""
    return path.read_text(encoding="utf-8")


def lexer_words() -> dict[str, set[str]]:
    """Extracts reserved words returned directly by the lexer."""
    groups = {"keywords": set(), "constants": set(), "operators": set()}
    pattern = re.compile(r'"((?:\\.|[^"\\])*)"\s*\{\s*return\s+([A-Z_]+);')

    for literal, token in pattern.findall(read(LEXER_TOKENS)):
        word = decode_cpp_string(literal)
        if not word.isidentifier():
            continue

        if token in CONSTANT_TOKENS:
            groups["constants"].add(word)
        elif token in OPERATOR_TOKENS:
            groups["operators"].add(word)
        else:
            groups["keywords"].add(word)

    return groups


def type_words() -> set[str]:
    """Extracts protected type words from interpreter source."""
    words = set(TYPE_WORDS)
    text = read(INTERPRETER)
    for literal in re.findall(r'kNumberType\w+\s*=\s*"((?:\\.|[^"\\])*)"', text):
        words.add(decode_cpp_string(literal))
    return words


def builtin_words() -> dict[str, set[str]]:
    """Extracts registered built-in names from built-in source files."""
    pattern = re.compile(r'k\w+(?:Builtin|Name)\w*\s*=\s*"((?:\\.|[^"\\])*)"')
    return {
        "core_builtins": {
            decode_cpp_string(literal)
            for literal in pattern.findall(read(CORE_BUILTINS))
        },
        "canvas_builtins": {
            decode_cpp_string(literal)
            for literal in pattern.findall(read(CANVAS_BUILTINS))
        },
    }


def collect_words() -> dict[str, list[str]]:
    """Collects all source words grouped for syntax highlighters."""
    lexer = lexer_words()
    builtins = builtin_words()
    groups = {
        "keywords": lexer["keywords"],
        "constants": lexer["constants"],
        "operators": lexer["operators"],
        "types": type_words(),
        "core_builtins": builtins["core_builtins"],
        "canvas_builtins": builtins["canvas_builtins"],
    }

    all_words = set()
    for words in groups.values():
        all_words.update(words)

    result = {name: unique_sorted(words) for name, words in groups.items()}
    result["all"] = unique_sorted(all_words)
    return result


def print_plain(groups: dict[str, list[str]]) -> None:
    """Prints grouped source words in a human-readable format."""
    for name, words in groups.items():
        print(f"[{name}]")
        for word in words:
            print(word)
        print()


def main() -> int:
    """Runs the keyword listing command."""
    parser = argparse.ArgumentParser(
        description="List plat-lang keywords and built-ins from source files."
    )
    parser.add_argument(
        "--format",
        choices=("plain", "json"),
        default="plain",
        help="Output format.",
    )
    parser.add_argument(
        "--output",
        type=Path,
        help="Write output to a file instead of stdout.",
    )
    args = parser.parse_args()

    groups = collect_words()
    if args.format == "json":
        output = json.dumps(groups, ensure_ascii=False, indent=2) + "\n"
    else:
        lines = []
        for name, words in groups.items():
            lines.append(f"[{name}]")
            lines.extend(words)
            lines.append("")
        output = "\n".join(lines)

    if args.output is None:
        print(output, end="")
    else:
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(output, encoding="utf-8")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
