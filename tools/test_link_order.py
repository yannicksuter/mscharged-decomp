import json
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

from link_order import align_script, read_alignments


class LinkOrderTest(unittest.TestCase):
    def test_split_sections_and_comments(self):
        splits = """Sections:
    .text type:code align:4
First.cpp:
    .text start:0x100 end:0x10C
Second.c:
    .text start:0x110 end:0x120 align:8 # input requirement
    .bss start:0x200 end:0x220 align:0x20
"""
        alignments = read_alignments(splits)
        self.assertEqual(alignments[".text"]["Second.c"], 8)
        self.assertEqual(alignments[".bss"], {"Second.c": 32})
        entries = [
            {"unit": "First.cpp", "output": "link/0000_memory.o"},
            {"unit": "Second.c", "output": "link/0001_memory.o"},
        ]
        script = align_script(
            ".text ALIGN(0x8):{}\n.bss ALIGN(0x40):{}\n.data ALIGN(0x20):{}",
            entries, alignments,
        )
        self.assertIn('"0000_memory.o" (.text)\n            . = ALIGN(8);\n'
                      '            "0001_memory.o" (.text)', script)
        self.assertIn(". = ALIGN(32);", script)
        self.assertIn(".data ALIGN(0x20):{}", script)

    def test_rejects_ambiguous_names(self):
        with self.assertRaisesRegex(ValueError, "basenames"):
            align_script("", [{"output": "a/memory.o"}, {"output": "b/memory.o"}], {})

    def test_rejects_invalid_alignment(self):
        for value in (0, 3, 12):
            with self.subTest(value=value), self.assertRaises(ValueError):
                read_alignments(f"file.c:\n    .text align:{value}\n")

    def test_preserves_existing_script_or_fails_closed(self):
        entry = [{"unit": "file.c", "output": "0000_file.o"}]
        script = ".text ALIGN(8):{ KEEP_SECTION(.text) }"
        self.assertEqual(align_script(script, entry, {}), script)
        with self.assertRaisesRegex(ValueError, "empty output-section"):
            align_script(script, entry, {".text": {"file.c": 8}})

    def test_command_preserves_object_bytes(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            entries = []
            for i in range(2):
                source = root / str(i) / "memory.o"
                source.parent.mkdir()
                source.write_bytes(bytes(range(256)) + bytes([i]))
                entries.append({"unit": f"{i}/memory.c", "source": str(source),
                                "output": str(root / "link" / f"{i:04d}_memory.o")})
            manifest = root / "inputs.json"
            manifest.write_text(json.dumps(entries))
            splits = root / "splits.txt"
            splits.write_text("1/memory.c:\n    .text align:8\n")
            script = root / "input.lcf"
            script.write_text(".text ALIGN(8):{}")
            output = root / "link" / "aligned.lcf"
            subprocess.run([sys.executable, str(Path(__file__).with_name("link_order.py")),
                            "--manifest", str(manifest), "--splits", str(splits),
                            "--script", str(script), "--output", str(output)], check=True)
            for entry in entries:
                self.assertEqual(Path(entry["source"]).read_bytes(),
                                 Path(entry["output"]).read_bytes())
            self.assertIn(". = ALIGN(8);", output.read_text())


if __name__ == "__main__":
    unittest.main()
