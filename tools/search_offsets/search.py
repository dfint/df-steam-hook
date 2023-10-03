from collections import defaultdict
from collections.abc import Iterable, Mapping
from operator import attrgetter
from pathlib import Path

import typer
from peclasses.portable_executable import PortableExecutable, SectionTable

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


def print_found(section_table: SectionTable, patternt_names: Iterable[str], found: Mapping[str, int]):
    for pattern in patternt_names:
        if not found[pattern]:
            print(f"{pattern}: NOT FOUND")
            continue

        for i, offset in enumerate(found[pattern], -1):
            suffix = "" if i < 0 else f"_{i}"
            name = pattern + suffix
            if name == "addchar_0":
                name = "addchar_top"

            rva = section_table.offset_to_rva(offset)
            print(f"{name} = 0x{rva:X}")


app = typer.Typer()


@app.command()
def main(path: Path):
    patterns = load_patterns()

    with open(path, "rb") as exe:
        pe = PortableExecutable(exe)
        print(f"checksum = 0x{pe.file_header.timedate_stamp:X}")
        section_table = pe.section_table

    found = search(path, patterns)
    print_found(section_table, map(attrgetter("name"), patterns), found)


if __name__ == "__main__":
    app()
