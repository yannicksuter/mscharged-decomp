"""Checks weak-provider selection and reference preservation with small ELF inputs."""

from pathlib import Path
import struct
import tempfile
import unittest

from objdiff_weak import Elf, combine


def fixture(path, symbols, payload, relocations=(), prefix=b""):
    # Each fixture has one allocated section, including deliberate prefixes to
    # check extraction from nonzero symbol offsets and section-relative addends.
    names = bytearray(b"\0")
    syms = bytearray(16)
    syms.extend(struct.pack(">IIIBBH", 0, 0, 0, 3, 0, 1))
    for name, value, size, binding in symbols:
        offset = len(names)
        names.extend(name.encode() + b"\0")
        syms.extend(struct.pack(">IIIBBH", offset, value, size, binding << 4 | 1, 0, 1))
    rel = b"".join(struct.pack(">IIi", off, index << 8 | 1, add) for off, index, add in relocations)
    section_names = b"\0.data\0.symtab\0.strtab\0.rela.data\0.shstrtab\0"
    parts = [b"", prefix + payload, syms, names, rel, section_names]
    descriptors = [(0, 0, 0, 0, 0, 0), (1, 1, 2, 4, 0, 0),
                   (7, 2, 0, 4, 3, 2), (15, 3, 0, 1, 0, 0),
                   (23, 4, 0, 4, 2, 1), (34, 3, 0, 1, 0, 0)]
    result = bytearray(52)
    headers = []
    for data, (name, kind, flags, align, link, info) in zip(parts, descriptors):
        while len(result) % max(align, 1):
            result.append(0)
        offset = len(result)
        result.extend(data)
        headers.append((name, kind, flags, 0, offset, len(data), link, info, align,
                        16 if kind == 2 else 12 if kind == 4 else 0))
    while len(result) % 4:
        result.append(0)
    start = len(result)
    for h in headers:
        result.extend(struct.pack(">10I", *h))
    struct.pack_into(">16sHHIIIIIHHHHHH", result, 0,
                     b"\x7fELF\x01\x02\x01" + b"\0" * 9,
                     1, 20, 1, 0, 0, start, 0, 52, 0, 0, 40, 6, 5)
    path.write_bytes(result)
    return Elf(path)


class WeakProviders(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.root = Path(self.tmp.name)

    def run_comparison(self, target, base, providers):
        output = self.root / "combined.o"
        combine(target, base, providers, output)
        obj = Elf(output)
        return {s[0]: (obj.sections[s[5]]["data"][s[1]:s[1] + s[2]], s)
                for s in obj.symbols if s[0] and obj.defined(s)}, obj

    def test_existing_definitions_win_and_unrequested_definitions_stay_out(self):
        target = fixture(self.root / "target", [("kept", 0, 4, 2), ("added", 4, 4, 2)], b"00001111")
        base = fixture(self.root / "base", [("kept", 0, 4, 1)], b"BASE")
        provider = fixture(self.root / "provider", [("kept", 0, 4, 2), ("added", 4, 4, 2),
                                                   ("other", 8, 4, 2)], b"FAKEGOODSKIP")
        definitions, _ = self.run_comparison(target, base, [provider])
        self.assertEqual(set(definitions), {"kept", "added"})
        self.assertEqual(definitions["kept"][0], b"BASE")
        self.assertEqual(definitions["added"][0], b"GOOD")
        self.assertEqual(definitions["added"][1][3] >> 4, 2)

    def test_strong_provider_does_not_fill_a_weak_target(self):
        target = fixture(self.root / "target", [("missing", 0, 4, 2)], b"0000")
        base = fixture(self.root / "base", [("base", 0, 4, 1)], b"BASE")
        provider = fixture(self.root / "provider", [("missing", 0, 4, 1)], b"NOPE")
        definitions, _ = self.run_comparison(target, base, [provider])
        self.assertEqual(set(definitions), {"base"})

    def test_rejects_implicit_addend_relocations(self):
        path = self.root / "provider"
        fixture(path, [("added", 0, 4, 2)], b"0000", [(0, 1, 0)])
        data = bytearray(path.read_bytes())
        sections = struct.unpack_from(">I", data, 32)[0]
        struct.pack_into(">I", data, sections + 4 * 40 + 4, 9)
        path.write_bytes(data)
        with self.assertRaisesRegex(ValueError, "RELA"):
            Elf(path)

    def test_anonymous_storage_and_section_addend_survive_extraction(self):
        target = fixture(self.root / "target", [("added", 0, 4, 2)], b"0000")
        base = fixture(self.root / "base", [("base", 0, 4, 1)], b"BASE")
        provider = fixture(self.root / "provider", [("literal", 4, 4, 0), ("added", 8, 4, 2)],
                           b"LIT!\0\0\0\0", [(8, 1, 4)], prefix=b"SKIP")
        definitions, obj = self.run_comparison(target, base, [provider])
        added = definitions["added"][1]
        site, index, kind, addend = obj.sections[added[5]]["rel"][0]
        destination = obj.symbols[index]
        self.assertEqual((site, kind), (0, 1))
        self.assertEqual(obj.sections[destination[5]]["data"][destination[1] + addend:][:4], b"LIT!")


if __name__ == "__main__":
    unittest.main()
