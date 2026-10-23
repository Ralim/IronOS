"""Use IronOS's existing ASCII glyphs in the standalone TS21 diagnostic."""

import argparse
from pathlib import Path
import runpy


def generate() -> str:
    root = Path(__file__).resolve().parents[5]
    fonts = runpy.run_path(str(root / "Translations" / "font_tables.py"))
    mapping = fonts["get_font_map_ascii_basic"]()
    lines = ["#include <stdint.h>", "static const uint8_t ts21_font[59][24] = {"]
    for code in range(32, 91):
        glyph = mapping[chr(code)]
        if len(glyph) != 24:
            raise ValueError(f"Unexpected glyph size for ASCII {code}")
        lines.append("  {" + ", ".join(f"0x{byte:02X}" for byte in glyph) + "},")
    lines.append("};")
    return "\n".join(lines) + "\n"


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("output", type=Path)
    args = parser.parse_args()
    args.output.write_text(generate(), encoding="ascii")


if __name__ == "__main__":
    main()
