#!/usr/bin/env python3
"""Extract compressed disc-message images into a generated C++ include."""

import argparse
import hashlib
import re
import struct
from pathlib import Path


TABLE_ADDRESSES = {
    "R4QE01": 0x804E70E0,
    "R4QP01_rev2": 0x804E6A08,
    "R4QP01_rev1": 0x804E6488,
}
LANGUAGES = ("eng", "fre", "deu", "spa", "ita", "jpn", "uke")
IMAGE_TYPES = ("FatalErrorImage", "NoDiskImage", "WrongDiskImage", "RetryImage", "Loading")


def read_data(dol: bytes, address: int, size: int) -> bytes:
    """Read one range from a loaded DOL data section."""
    for index in range(7, 18):
        offset = struct.unpack_from(">I", dol, index * 4)[0]
        start = struct.unpack_from(">I", dol, 0x48 + index * 4)[0]
        length = struct.unpack_from(">I", dol, 0x90 + index * 4)[0]
        if start <= address and address + size <= start + length:
            first = offset + address - start
            if first + size <= len(dol):
                return dol[first:first + size]
    raise ValueError(f"Image range 0x{address:08X}+0x{size:X} is outside the DOL data sections")


def extract(dol: bytes, version: str) -> str:
    table = read_data(dol, TABLE_ADDRESSES[version], len(LANGUAGES) * len(IMAGE_TYPES) * 16)
    images = []
    for language_index, language in enumerate(LANGUAGES):
        for image_index, image_type in enumerate(IMAGE_TYPES):
            offset = (language_index * len(IMAGE_TYPES) + image_index) * 16
            width, height, size, address = struct.unpack_from(">4I", table, offset)
            if not width or width % 8 or not height or not size:
                raise ValueError(f"Invalid image descriptor for {image_type}_{language}")
            images.append((address, f"HEADER_{image_type}_{language}", read_data(dol, address, size)))

    lines = ["// Generated from the local original executable. Do not commit this file.", ""]
    for _, namespace, data in sorted(images):
        lines += [f"namespace {namespace}", "{", f"const unsigned char g_ImageData[0x{len(data):X}] = {{"]
        for offset in range(0, len(data), 24):
            lines.append("    " + ", ".join(f"0x{byte:02X}" for byte in data[offset:offset + 24]) + ",")
        lines += ["};", "}", ""]
    return "\n".join(lines)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--version", required=True, choices=TABLE_ADDRESSES)
    parser.add_argument("dol", type=Path)
    parser.add_argument("output", type=Path)
    args = parser.parse_args()
    try:
        dol = args.dol.read_bytes()
        sha1_path = Path(__file__).resolve().parents[1] / "config" / args.version / "build.sha1"
        hashes = sha1_path.read_text().split()
        if not hashes or not re.fullmatch(r"[0-9a-fA-F]{40}", hashes[0]):
            raise ValueError(f"Invalid SHA-1 file: {sha1_path}")
        if hashlib.sha1(dol).hexdigest() != hashes[0].lower():
            raise ValueError(f"Original executable does not match {args.version}")
        generated = extract(dol, args.version)
        if not args.output.exists() or args.output.read_text() != generated:
            args.output.parent.mkdir(parents=True, exist_ok=True)
            args.output.write_text(generated)
    except (OSError, ValueError, struct.error) as error:
        parser.error(str(error))


if __name__ == "__main__":
    main()
