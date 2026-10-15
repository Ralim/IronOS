"""Check TS21 diagnostic ELF/HEX contents, not their hardware compatibility."""

import argparse
import hashlib
import json
from pathlib import Path
import struct
import sys
from typing import Dict, Optional


APP_START = 0x08007000
FLASH_END = 0x0800FC00
RAM_START = 0x20000000
STACK_BOTTOM = 0x20004800
VECTOR_BASE = APP_START + 4
VECTOR_COUNT = 64
ELF_HEADER = struct.Struct("<16sHHIIIIIHHHHHH")
PROGRAM_HEADER = struct.Struct("<IIIIIIII")


def require(condition: bool, message: str) -> None:
    if not condition:
        raise ValueError(message)


def parse_hex(data: bytes) -> Dict[int, int]:
    """Accept the Intel HEX record types used by the TS21 stock image and objcopy."""
    lines = data.decode("ascii").replace("\r\n", "\n").split("\n")
    if lines[-1] == "":
        lines.pop()
    contents = {}
    base = 0
    next_address = APP_START
    ended = False
    start_seen = False
    for number, line in enumerate(lines, 1):
        require(not ended, "HEX content after EOF")
        require(
            len(line) >= 11
            and len(line) % 2 == 1
            and line[0] == ":"
            and all(character in "0123456789abcdefABCDEF" for character in line[1:]),
            f"Malformed HEX record at line {number}",
        )
        record = bytes.fromhex(line[1:])
        count = record[0]
        require(len(record) == count + 5, f"HEX byte count mismatch at line {number}")
        require(sum(record) & 0xFF == 0, f"HEX checksum mismatch at line {number}")
        address = int.from_bytes(record[1:3], "big")
        kind = record[3]
        payload = record[4:-1]
        if kind == 0:
            require(count > 0 and address + count <= 0x10000, "Invalid HEX data range")
            first = base + address
            require(
                APP_START <= first and first + count <= FLASH_END,
                "HEX data overwrites bootloader or persistent settings",
            )
            require(first >= next_address, "Overlapping or out-of-order HEX data")
            contents.update((first + index, byte) for index, byte in enumerate(payload))
            next_address = first + count
        elif kind == 1:
            require(count == 0 and address == 0, "Invalid HEX EOF record")
            ended = True
        elif kind == 4:
            require(count == 2 and address == 0, "Invalid HEX address record")
            base = int.from_bytes(payload, "big") << 16
            require(
                base == APP_START & 0xFFFF0000,
                "HEX address bank is outside application flash",
            )
        elif kind == 5:
            require(
                count == 4 and address == 0 and not start_seen,
                "Invalid HEX start-address record",
            )
            require(
                int.from_bytes(payload, "big") == APP_START,
                "HEX entry must be 0x08007000",
            )
            start_seen = True
        else:
            raise ValueError(f"Unsupported HEX record type {kind} at line {number}")
    require(ended, "Missing HEX EOF record")
    require(bool(contents), "Missing HEX data")
    return contents


def inspect(data: bytes, hex_data: Optional[bytes] = None) -> dict:
    require(len(data) >= ELF_HEADER.size, "Truncated ELF header")
    header = ELF_HEADER.unpack_from(data)
    (
        ident,
        kind,
        machine,
        version,
        entry,
        phoff,
        _shoff,
        flags,
        ehsize,
        phentsize,
        phnum,
        _shentsize,
        _shnum,
        _shstrndx,
    ) = header
    require(ident[:7] == b"\x7fELF\x01\x01\x01", "Expected little-endian ELF32")
    require(
        kind == 2 and machine == 243 and version == 1,
        "Expected an executable RISC-V ELF",
    )
    require(flags & 0xE == 0, "Expected full-register, soft-float RV32 ABI")
    require(ehsize == ELF_HEADER.size, "Unexpected ELF header size")
    require(entry == APP_START, "Entry point must preserve the 28 KiB bootloader")
    require(
        phentsize == PROGRAM_HEADER.size and phnum > 0, "Invalid program header table"
    )
    require(
        phoff >= ehsize and phoff + phnum * phentsize <= len(data),
        "Truncated program header table",
    )

    flash_segments = []
    ram_ranges = []
    executable_ranges = []
    for index in range(phnum):
        segment = PROGRAM_HEADER.unpack_from(data, phoff + index * phentsize)
        ptype, offset, vaddr, paddr, filesz, memsz, permissions, alignment = segment
        if ptype != 1:
            continue
        require(filesz <= memsz, "Segment file size exceeds memory size")
        require(offset + filesz <= len(data), "Segment extends past the input file")
        require(
            alignment in (0, 1) or alignment & (alignment - 1) == 0,
            "Invalid segment alignment",
        )
        if alignment > 1:
            require(offset % alignment == vaddr % alignment, "Misaligned load segment")
        if RAM_START <= vaddr < STACK_BOTTOM:
            require(
                vaddr + memsz <= STACK_BOTTOM, "RAM overlaps the reserved 2 KiB stack"
            )
            ram_ranges.append((vaddr, vaddr + memsz))
        else:
            require(
                APP_START <= vaddr and vaddr + memsz <= FLASH_END,
                "Virtual load address is outside application flash/RAM",
            )
        if filesz:
            require(
                APP_START <= paddr and paddr + filesz <= FLASH_END,
                "Image overwrites bootloader or persistent settings",
            )
            flash_segments.append((paddr, paddr + filesz, offset))
        if permissions & 1:
            require(
                APP_START <= vaddr and vaddr + filesz <= FLASH_END,
                "Executable segment is not in application flash",
            )
            require(
                vaddr == paddr, "Executable segment has a different physical address"
            )
            executable_ranges.append((vaddr, vaddr + filesz))

    require(
        bool(flash_segments) and bool(executable_ranges),
        "Missing executable flash contents",
    )
    for ranges in (flash_segments, ram_ranges):
        ordered = sorted(ranges)
        for previous, current in zip(ordered, ordered[1:]):
            require(previous[1] <= current[0], "Overlapping load segments")

    def flash_word(address: int) -> int:
        for start, end, offset in flash_segments:
            if start <= address and address + 4 <= end:
                return struct.unpack_from("<I", data, offset + address - start)[0]
        raise ValueError(f"Missing flash word at 0x{address:08X}")

    def executable(address: int) -> bool:
        return address % 2 == 0 and any(
            start <= address and address + 2 <= end for start, end in executable_ranges
        )

    instruction = flash_word(entry)
    require(instruction & 0xFFF == 0x06F, "Entry must be a 32-bit JAL x0 instruction")
    displacement = (
        ((instruction >> 31) & 1) << 20
        | ((instruction >> 12) & 0xFF) << 12
        | ((instruction >> 20) & 1) << 11
        | ((instruction >> 21) & 0x3FF) << 1
    )
    if displacement & (1 << 20):
        displacement -= 1 << 21
    reset_handler = entry + displacement
    require(executable(reset_handler), "Reset jump leaves executable application flash")
    require(flash_word(VECTOR_BASE) == entry, "Invalid reset vector at 0x08007004")
    for index in range(1, VECTOR_COUNT):
        require(
            executable(flash_word(VECTOR_BASE + index * 4)),
            f"Invalid interrupt vector {index}",
        )

    report = {
        "status": "layout-valid; hardware-unvalidated",
        "entry": f"0x{entry:08X}",
        "reset_handler": f"0x{reset_handler:08X}",
        "vector_base": f"0x{VECTOR_BASE:08X}",
        "flash_end_exclusive": f"0x{max(end for _, end, _ in flash_segments):08X}",
        "flash_loaded_bytes": sum(end - start for start, end, _ in flash_segments),
        "flash_budget_bytes": FLASH_END - APP_START,
        "ram_static_bytes": sum(end - start for start, end in ram_ranges),
        "stack_reserved_bytes": 2048,
        "sha256": hashlib.sha256(data).hexdigest(),
    }
    if hex_data is not None:
        expected = {
            start + index: byte
            for start, end, offset in flash_segments
            for index, byte in enumerate(data[offset : offset + end - start])
        }
        contents = parse_hex(hex_data)
        require(contents == expected, "HEX contents do not match the validated ELF")
        report["hex_loaded_bytes"] = len(contents)
        report["hex_sha256"] = hashlib.sha256(hex_data).hexdigest()
    return report


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("elf", type=Path)
    parser.add_argument(
        "--hex",
        dest="hex_file",
        type=Path,
        help="Verify an Intel HEX export against the ELF",
    )
    args = parser.parse_args()
    try:
        hex_data = args.hex_file.read_bytes() if args.hex_file is not None else None
        report = inspect(args.elf.read_bytes(), hex_data)
    except (OSError, ValueError, struct.error) as error:
        print(f"TS21 image rejected: {error}", file=sys.stderr)
        return 1
    print(json.dumps(report, indent=2))
    return 0


if __name__ == "__main__":
    sys.exit(main())
