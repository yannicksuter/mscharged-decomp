"""Apply splits.txt input-section alignment to a CodeWarrior DOL link.

DTK applies split alignments to extracted objects. Compiled objects may require
the same alignment from the linker. Keep their bytes intact and express those
requirements with the LCF location counter. Input names must be unique because
CodeWarrior's file selectors compare basenames, even when given full paths.
"""

import argparse
import json
from pathlib import Path
import re
import shutil


def read_alignments(text):
    alignments = {}
    unit = None
    for original in text.splitlines():
        line = original.split("#", 1)[0].rstrip()
        if not line:
            continue
        if not line[0].isspace() and line.endswith(":"):
            unit = line[:-1]
        elif unit and line[0].isspace():
            match = re.search(r"\balign:(0x[0-9a-fA-F]+|[0-9]+)\b", line)
            if match:
                alignment = int(match[1], 16 if match[1].startswith("0x") else 10)
                if alignment <= 0 or alignment & (alignment - 1):
                    raise ValueError(f"Invalid alignment for {unit}: {alignment}")
                alignments.setdefault(line.split()[0], {})[unit] = alignment
    return alignments


def align_script(script, entries, alignments):
    names = [Path(entry["output"]).name for entry in entries]
    if len(names) != len(set(names)):
        raise ValueError("LCF input basenames must be unique")
    for section, units in alignments.items():
        if not any(entry["unit"] in units for entry in entries):
            continue
        body = []
        for entry, name in zip(entries, names):
            if entry["unit"] in units:
                body.append(f"            . = ALIGN({units[entry['unit']]});")
            body.append(f'            "{name}" ({section})')
        pattern = r"(" + re.escape(section) + r"\s+ALIGN\([^\n]+?\):)\{\}"
        script, count = re.subn(
            pattern, lambda match: match[1] + "{\n" + "\n".join(body) + "\n        }", script
        )
        if count != 1:
            raise ValueError(f"Expected one empty output-section definition for {section}")
    return script


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--manifest", type=Path, required=True)
    parser.add_argument("--splits", type=Path, required=True)
    parser.add_argument("--script", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    entries = json.loads(args.manifest.read_text())
    script = align_script(
        args.script.read_text(), entries, read_alignments(args.splits.read_text())
    )
    for entry in entries:
        destination = Path(entry["output"])
        destination.parent.mkdir(parents=True, exist_ok=True)
        shutil.copyfile(entry["source"], destination)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(script)


if __name__ == "__main__":
    main()
