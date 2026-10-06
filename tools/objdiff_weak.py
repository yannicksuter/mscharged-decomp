#!/usr/bin/env python3
"""Add missing weak definitions from compiled providers to an objdiff-only object.

The original source objects remain the linker inputs. Target names select the
surface to compare, but every added byte and relocation comes from a provider.
"""

import argparse
from pathlib import Path
import struct


class Elf:
    def __init__(self, path):
        data = path.read_bytes()
        self.header = struct.unpack_from(">16sHHIIIIIHHHHHH", data)
        if data[:6] != b"\x7fELF\x01\x02" or self.header[1:3] != (1, 20):
            raise ValueError(f"{path}: expected a big-endian PPC relocatable ELF32")
        self.sections = []
        for index in range(self.header[12]):
            h = struct.unpack_from(">10I", data, self.header[6] + index * 40)
            payload = data[h[4]:h[4] + h[5]] if h[1] != 8 else b""
            self.sections.append({"h": h, "data": payload, "rel": []})
        strings = self.sections[self.header[13]]["data"]
        for section in self.sections:
            section["name"] = self.string(strings, section["h"][0])
        tables = []
        for index, section in enumerate(self.sections):
            h = section["h"]
            if h[1] == 2:
                tables.append(index)
                symbols = []
                names = self.sections[h[6]]["data"]
                for offset in range(0, h[5], 16):
                    n, value, size, info, other, owner = struct.unpack_from(
                        ">IIIBBH", section["data"], offset
                    )
                    symbols.append((self.string(names, n), value, size, info, other, owner))
                self.symbols = symbols
        if len(tables) != 1:
            raise ValueError(f"{path}: expected one symbol table")
        for section in self.sections:
            h = section["h"]
            if h[1] == 9 and self.sections[h[7]]["h"][2] & 2:
                raise ValueError(f"{path}: expected explicit-addend RELA relocations")
            if h[1] == 4 and self.sections[h[7]]["h"][2] & 2:
                if h[6] != tables[0]:
                    raise ValueError(f"{path}: unexpected relocation symbol table")
                for offset in range(0, h[5], 12):
                    site, info, addend = struct.unpack_from(">IIi", section["data"], offset)
                    self.sections[h[7]]["rel"].append((site, info >> 8, info & 255, addend))

    @staticmethod
    def string(data, offset):
        return data[offset:data.index(0, offset)].decode("ascii")

    def defined(self, symbol):
        owner = symbol[5]
        return 0 < owner < len(self.sections) and self.sections[owner]["h"][2] & 2


def combine(target, base, providers, output):
    objects = [base, *providers]
    definitions = {s[0] for s in base.symbols if base.defined(s) and s[3] >> 4}
    wanted = {s[0] for s in target.symbols if target.defined(s) and s[3] >> 4 == 2}
    selected = []
    for obj in providers:
        for symbol in obj.symbols:
            if (symbol[0] in wanted and symbol[0] not in definitions
                    and obj.defined(symbol) and symbol[3] >> 4 == 2):
                selected.append((obj, symbol[5], symbol[1], symbol[1] + symbol[2]))
                definitions.add(symbol[0])

    # Keep each selected definition's actual bytes, plus referenced anonymous
    # data. Original base sections and symbol positions are preserved in full.
    sections = [(base, i, 0, s["h"][5]) for i, s in enumerate(base.sections)
                if s["h"][2] & 2]
    pending = list(selected)
    while pending:
        key = pending.pop(0)
        if key in sections:
            continue
        obj, index, start, end = key
        if start == end:
            raise ValueError("weak definition has no recoverable extent")
        sections.append(key)
        for site, symbol_index, _, addend in obj.sections[index]["rel"]:
            if not start <= site < end:
                continue
            symbol = obj.symbols[symbol_index]
            if obj.defined(symbol) and symbol[3] >> 4 == 0:
                point = symbol[1] + addend
                owner = next((s for s in obj.symbols if s[5] == symbol[5]
                              and s[2] and s[1] <= point < s[1] + s[2]), None)
                pending.append((obj, symbol[5], owner[1] if owner else 0,
                                owner[1] + owner[2] if owner else
                                obj.sections[symbol[5]]["h"][5]))
    indices = {key: i + 1 for i, key in enumerate(sections)}

    def location(obj, owner, point):
        return next((key for key in sections if key[0] is obj and key[1] == owner
                     and key[2] <= point < key[3]), None)

    needed = {obj: set() for obj in objects}
    for obj, index, start, end in sections:
        needed[obj].update(i for site, i, _, _ in obj.sections[index]["rel"]
                           if start <= site < end)
    entries = []
    origins = []
    global_indices = {}
    for obj in objects:
        for i, symbol in enumerate(obj.symbols):
            if i == 0:
                continue
            if obj is not base and i not in needed[obj] and not (
                    obj.defined(symbol) and location(obj, symbol[5], symbol[1])):
                continue
            name, value, size, info, other, owner = symbol
            if obj.defined(symbol):
                key = location(obj, owner, value)
                if key is None:
                    if not info >> 4:
                        continue
                    value = size = owner = 0
                else:
                    value -= key[2]
                    owner = indices[key]
            elif owner not in (0, 0xFFF1):
                continue
            if info >> 4 and name in global_indices:
                continue
            if info >> 4:
                global_indices[name] = len(entries)
            entries.append((name, value, size, info, other, owner))
            origins.append((obj, i))
    # Retained definitions win over undefined declarations with the same name.
    for key in sections:
        obj, index, start, end = key
        for i, symbol in enumerate(obj.symbols):
            if symbol[5] != index or not symbol[3] >> 4 or not start <= symbol[1] < end:
                continue
            existing = global_indices[symbol[0]]
            if entries[existing][5] != 0:
                continue
            entries[existing] = (symbol[0], symbol[1] - start, *symbol[2:5], indices[key])
            origins[existing] = (obj, i)
    section_symbols = {}
    for key in sections:
        section_symbols[key] = len(entries)
        entries.append(("", 0, 0, 3, 0, indices[key]))
        origins.append(None)
    order = sorted(range(len(entries)), key=lambda i: entries[i][3] >> 4 != 0)
    final = [("", 0, 0, 0, 0, 0), *(entries[i] for i in order)]
    symbol_map = {origins[i]: j + 1 for j, i in enumerate(order) if origins[i] is not None}
    ordered_indices = {i: j + 1 for j, i in enumerate(order)}
    section_symbols = {key: ordered_indices[i] for key, i in section_symbols.items()}
    globals_final = {s[0]: i for i, s in enumerate(final) if s[3] >> 4}
    strings = bytearray(b"\0")
    symbol_data = bytearray()
    for name, value, size, info, other, owner in final:
        offset = len(strings) if name else 0
        if name:
            strings.extend(name.encode("ascii") + b"\0")
        symbol_data.extend(struct.pack(">IIIBBH", offset, value, size, info, other, owner))
    out = [("", 0, 0, 0, b"", 0, 0, 0, 0)]
    for obj, index, start, end in sections:
        s = obj.sections[index]
        h = s["h"]
        out.append((s["name"], h[1], h[2], end - start, s["data"][start:end], h[8], 0, 0, h[9]))
    symtab = len(out)
    local_count = sum(s[3] >> 4 == 0 for s in final)
    out.append((".symtab", 2, 0, len(symbol_data), symbol_data, 4,
                symtab + 1, local_count, 16))
    out.append((".strtab", 3, 0, len(strings), strings, 1, 0, 0, 0))
    for key in sections:
        obj, index, start, end = key
        s = obj.sections[index]
        if not s["rel"]:
            continue
        payload = bytearray()
        for site, old, kind, addend in s["rel"]:
            if not start <= site < end:
                continue
            symbol = obj.symbols[old]
            if old == 0:
                new = 0
            elif symbol[3] >> 4:
                new = globals_final[symbol[0]]
            elif obj is not base and obj.defined(symbol):
                destination = symbol[1] + addend
                owner = location(obj, symbol[5], destination)
                if owner is None:
                    raise ValueError("anonymous relocation has no selected storage")
                new = section_symbols[owner]
                addend = destination - owner[2]
            else:
                new = symbol_map[obj, old]
            payload.extend(struct.pack(">IIi", site - start, new << 8 | kind, addend))
        out.append((".rela" + s["name"], 4, 0, len(payload), payload,
                    4, symtab, indices[key], 12))
    shstr = bytearray(b"\0")
    name_offsets = []
    for name, *_ in out:
        name_offsets.append(len(shstr) if name else 0)
        if name:
            shstr.extend(name.encode("ascii") + b"\0")
    name_offsets.append(len(shstr))
    shstr.extend(b".shstrtab\0")
    out.append((".shstrtab", 3, 0, len(shstr), shstr, 1, 0, 0, 0))
    result = bytearray(52)
    headers = []
    for n, (_, kind, flags, size, data, alignment, link, info, stride) in zip(name_offsets, out):
        while len(result) % max(alignment, 1):
            result.append(0)
        offset = len(result)
        result.extend(data)
        headers.append((n, kind, flags, 0, offset, size, link, info, alignment, stride))
    while len(result) % 4:
        result.append(0)
    start = len(result)
    for h in headers:
        result.extend(struct.pack(">10I", *h))
    header = list(base.header)
    header[5] = 0
    header[6] = start
    header[9] = header[10] = 0
    header[12] = len(headers)
    header[13] = len(headers) - 1
    struct.pack_into(">16sHHIIIIIHHHHHH", result, 0, *header)
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_bytes(result)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--target", type=Path, required=True)
    parser.add_argument("--base", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("providers", type=Path, nargs="+")
    args = parser.parse_args()
    combine(Elf(args.target), Elf(args.base), [Elf(p) for p in args.providers], args.output)


if __name__ == "__main__":
    main()
