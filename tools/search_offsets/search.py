from collections import defaultdict
from operator import attrgetter
from pathlib import Path
from collections.abc import Iterable, Mapping

import typer

from search_offsets.patterns import (
    Pattern,
    check_pattern,
    group_patterns,
    load_patterns,
)


def search(path: str, patterns: list[Pattern]):
    patterns_dict = group_patterns(patterns)

    found = defaultdict(list)

    with open(path, "rb") as file:
        data = file.read()
        for i, c in enumerate(data):
            possible_patterns = patterns_dict.get(c)
            if possible_patterns:
                for pattern in possible_patterns:
                    if check_pattern(data, i, pattern.pattern):
                        found[pattern.name].append(i)

    return found


def print_found(patternt_names: Iterable[str], found: Mapping[str, int]):
    diff_to_rva = 0xC00

    for pattern in patternt_names:
        if not found[pattern]:
            print(f"{pattern}: NOT FOUND")
            continue

        for i, offset in enumerate(found[pattern], -1):
            suffix = "" if i < 0 else f"_{i}"
            name = pattern + suffix
            if name == "addchar_0":
                name = "addchar_top"
            print(f"{name} = 0x{offset + diff_to_rva:X}")


app = typer.Typer()


@app.command()
def main(path: Path):
    patterns = load_patterns()
    found = search(path, patterns)
    print_found(map(attrgetter("name"), patterns), found)


if __name__ == "__main__":
    app()
