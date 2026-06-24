#!/usr/bin/env python3
# Pack a raw firmware .bin into an Alientek T90 ".atk" image for its USB bootloader.
#
# Format reverse-engineered + byte-for-byte round-trip verified against two real T90 .atk samples
# (the official factory t90.atk and PanKleszcz's t90_msdmem.atk):
#
#   header (13 bytes):
#     [0:6]  magic            = 08 02 0A 01 A0 20
#     [6:10] payload size      = uint32 little-endian (length of the app image)
#     [10]   year - 2000       = uint8   (build date, metadata only)
#     [11]   month             = uint8
#     [12]   day               = uint8
#   payload: the raw application image with every byte XOR 0xFF (the only "encryption").
#
# There is no payload checksum or signature in the header; the bootloader accepts any correctly
# formatted image (the per-USB-packet CRC of the transfer protocol is separate). The application is
# linked at 0x08005000 (the 20 KB bootloader lives below it).
#
# Stdlib only (mirrors dfuse-pack.py), so it runs in the CI build container with plain python3.

import argparse
import struct
import sys

MAGIC = bytes([0x08, 0x02, 0x0A, 0x01, 0xA0, 0x20])


def pack(raw: bytes, year: int, month: int, day: int) -> bytes:
    header = MAGIC + struct.pack("<I", len(raw)) + bytes([(year - 2000) & 0xFF, month & 0xFF, day & 0xFF])
    payload = bytes(b ^ 0xFF for b in raw)
    return header + payload


def main() -> int:
    ap = argparse.ArgumentParser(description="Pack a raw .bin into an Alientek T90 .atk image")
    ap.add_argument("input", help="raw firmware .bin (linked at 0x08005000)")
    ap.add_argument("output", help="output .atk path")
    ap.add_argument("--date", default="2000-01-01", help="build date YYYY-MM-DD stamped in the header (metadata only)")
    args = ap.parse_args()

    try:
        year, month, day = (int(x) for x in args.date.split("-"))
    except ValueError:
        print("--date must be YYYY-MM-DD", file=sys.stderr)
        return 2

    with open(args.input, "rb") as f:
        raw = f.read()
    with open(args.output, "wb") as f:
        f.write(pack(raw, year, month, day))
    print(f"wrote {args.output}: 13-byte header + {len(raw)} bytes payload (XOR 0xFF)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
