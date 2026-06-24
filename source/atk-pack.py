#!/usr/bin/env python3
# Pack a raw firmware .bin into an Alientek ".atk" image for the T90 USB bootloader.
#
# The .atk format is a 32-byte little-endian header followed by the raw application image:
#   name[16] data_len(B) encrypt_pos(B) app_addr(H) app_ver(H) crc(H) fw_size(I) year(H) month(B) day(B)
# The Alientek bootloader optionally XOR-obfuscates the payload, keyed off the byte at
# `encrypt_pos`. Setting encrypt_pos = 0xFF (> the per-chunk size) disables that step, so the
# payload is sent/stored verbatim (see the dozed-dev/atk-fw-util reverse-engineering). We ship the
# payload plain, so no checksum is required (crc = 0).
#
# Stdlib only (mirrors dfuse-pack.py), so it runs in the CI build container with plain python3.
#
# NOTE (must verify on hardware before flashing): the `name` and `app_addr` header fields are taken
# from the reverse-engineered T80/T90 update format. Confirm them against an official ATK-T90 update
# file (decrypt its header) before relying on this image to flash; the universal artifacts are the
# .hex/.bin. Override the name with --name if a real T90 .atk uses a different identifier.

import argparse
import struct
import sys

HEADER_FORMAT = "<16s B B H H H I H B B"
HEADER_SIZE = 32
CHUNK_SIZE = 58  # per-packet payload size the updater streams; also stored as data_len
ENCRYPT_DISABLED = 0xFF  # > CHUNK_SIZE -> bootloader skips the XOR step (plain payload)


def pack(raw: bytes, name: str) -> bytes:
    name_bytes = name.encode("ascii")[:16]
    header = struct.pack(
        HEADER_FORMAT,
        name_bytes,        # name[16], null-padded by struct
        CHUNK_SIZE,        # data_len
        ENCRYPT_DISABLED,  # encrypt_pos (0xFF -> no XOR)
        1,                 # app_addr (loader-relative index; bootloader places the app)
        1,                 # app_ver
        0,                 # crc (payload is plain, no checksum)
        len(raw),          # fw_size
        0, 0, 0,           # year, month, day (zeroed for reproducible builds)
    )
    return header + raw


def main() -> int:
    ap = argparse.ArgumentParser(description="Pack a raw .bin into an Alientek .atk image")
    ap.add_argument("input", help="raw firmware .bin")
    ap.add_argument("output", help="output .atk path")
    ap.add_argument("--name", default="ATK-PTT90", help="16-char device name stamped in the header")
    args = ap.parse_args()

    with open(args.input, "rb") as f:
        raw = f.read()
    with open(args.output, "wb") as f:
        f.write(pack(raw, args.name))
    print(f"wrote {args.output}: {HEADER_SIZE}-byte header + {len(raw)} bytes payload")
    return 0


if __name__ == "__main__":
    sys.exit(main())
