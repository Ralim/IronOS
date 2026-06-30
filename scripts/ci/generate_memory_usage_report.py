#!/usr/bin/env python3
"""Generate a markdown memory-usage report from ELF files using objdump.

This script:
- scans ELF files in source/Hexfile
- runs `objdump -h` on each ELF
- computes flash and RAM usage by summing ALLOC sections in address ranges
- groups entries by device model
- emits a markdown report with min/max flash builds per model
"""

from __future__ import annotations

import argparse
import json
import pathlib
import re
import subprocess
import sys
from dataclasses import dataclass
from typing import Dict, Iterable, List, Tuple


@dataclass(frozen=True)
class ModelConfig:
    flash_total: int
    ram_total: int
    flash_ranges: Tuple[Tuple[int, int], ...]
    ram_ranges: Tuple[Tuple[int, int], ...]


# NOTE:
# Totals are chosen to match the effective app/linker regions used by each model.
# Ranges are used to classify section addresses from objdump output.
MODEL_CONFIG: Dict[str, ModelConfig] = {
    # Miniware family
    "TS100": ModelConfig(
        flash_total=46 * 1024,
        ram_total=20 * 1024,
        flash_ranges=((0x08000000, 0x08100000),),
        ram_ranges=((0x20000000, 0x20005000),),
    ),
    "TS80": ModelConfig(
        flash_total=46 * 1024,
        ram_total=20 * 1024,
        flash_ranges=((0x08000000, 0x08100000),),
        ram_ranges=((0x20000000, 0x20005000),),
    ),
    "TS80P": ModelConfig(
        flash_total=46 * 1024,
        ram_total=20 * 1024,
        flash_ranges=((0x08000000, 0x08100000),),
        ram_ranges=((0x20000000, 0x20005000),),
    ),
    "TS101": ModelConfig(
        flash_total=72 * 1024,
        ram_total=20 * 1024,
        flash_ranges=((0x08000000, 0x08100000),),
        ram_ranges=((0x20000000, 0x20005000),),
    ),
    # Sequre family
    "S60": ModelConfig(
        flash_total=45 * 1024,
        ram_total=20 * 1024,
        flash_ranges=((0x08000000, 0x08100000),),
        ram_ranges=((0x20000000, 0x20005000),),
    ),
    "T55": ModelConfig(
        flash_total=45 * 1024,
        ram_total=20 * 1024,
        flash_ranges=((0x08000000, 0x08100000),),
        ram_ranges=((0x20000000, 0x20005000),),
    ),
    "S60P": ModelConfig(
        flash_total=78336,
        ram_total=20 * 1024,
        flash_ranges=((0x08000000, 0x08100000),),
        ram_ranges=((0x20000000, 0x20005000),),
    ),
    # MHP30
    "MHP30": ModelConfig(
        flash_total=94 * 1024,
        ram_total=20 * 1024,
        flash_ranges=((0x08000000, 0x08100000),),
        ram_ranges=((0x20000000, 0x20005000),),
    ),
    # Pinecil v1
    "Pinecil": ModelConfig(
        flash_total=0x1F400,
        ram_total=0x6800,
        flash_ranges=((0x08000000, 0x08100000),),
        ram_ranges=((0x20000000, 0x20006800),),
    ),
    # Pinecil v2 (BL702)
    "Pinecilv2": ModelConfig(
        flash_total=1008 * 1024,
        # dtcm (8K) + ram (72K) + ram2 (31K-8K) + hbn (0xE00)
        ram_total=(8 * 1024) + (72 * 1024) + (23 * 1024) + 0xE00,
        flash_ranges=((0x23000000, 0x23200000),),
        ram_ranges=(
            (0x42014000, 0x42016000),  # dtcm_memory
            (0x42016000, 0x42028000),  # ram_memory
            (0x42028400, 0x4202E000),  # ram2_memory
            (0x40010000, 0x40010E00),  # hbn_memory
            (0x22014000, 0x22017000),  # itcm_memory
        ),
    ),
}

SECTION_RE = re.compile(
    r"^\s*\d+\s+(\S+)\s+([0-9A-Fa-f]+)\s+([0-9A-Fa-f]+)\s+([0-9A-Fa-f]+)\s+([0-9A-Fa-f]+)\s+\d+\*\*\d+\s*$"
)


@dataclass
class BuildUsage:
    model: str
    build: str
    flash_used: int
    ram_used: int


def in_ranges(addr: int, ranges: Iterable[Tuple[int, int]]) -> bool:
    for start, end in ranges:
        if start <= addr < end:
            return True
    return False


def run_objdump(elf_path: pathlib.Path) -> str:
    proc = subprocess.run(
        ["objdump", "-h", str(elf_path)],
        text=True,
        capture_output=True,
        check=False,
    )
    if proc.returncode != 0:
        raise RuntimeError(f"objdump failed for {elf_path}: {proc.stderr.strip()}")
    return proc.stdout


def parse_objdump_sections(text: str) -> List[Tuple[str, int, int, int, List[str]]]:
    """Return list of (name, size, vma, lma, flags)."""
    lines = text.splitlines()
    sections: List[Tuple[str, int, int, int, List[str]]] = []

    i = 0
    while i < len(lines):
        match = SECTION_RE.match(lines[i])
        if not match:
            i += 1
            continue

        name = match.group(1)
        size = int(match.group(2), 16)
        vma = int(match.group(3), 16)
        lma = int(match.group(4), 16)

        flags: List[str] = []
        if i + 1 < len(lines):
            # flags are printed on the next line, comma-separated
            flags_line = lines[i + 1].strip()
            if flags_line:
                flags = [f.strip() for f in flags_line.split(",") if f.strip()]

        sections.append((name, size, vma, lma, flags))
        i += 2

    return sections


def parse_filename(elf_path: pathlib.Path) -> Tuple[str, str]:
    stem = elf_path.stem
    if "_" not in stem:
        raise ValueError(f"Unexpected ELF filename format: {elf_path.name}")
    model, build = stem.split("_", 1)
    return model, build


def pretty_bytes(n: int) -> str:
    if n < 1024:
        return f"{n} B"
    return f"{n / 1024:.1f} KiB"


def pct(used: int, total: int) -> float:
    if total <= 0:
        return 0.0
    return (used / total) * 100.0


def compute_usage(elf_path: pathlib.Path) -> BuildUsage:
    model, build = parse_filename(elf_path)
    if model not in MODEL_CONFIG:
        raise KeyError(f"No model config for {model} (file: {elf_path.name})")

    config = MODEL_CONFIG[model]
    out = run_objdump(elf_path)
    sections = parse_objdump_sections(out)

    flash_used = 0
    ram_used = 0

    for _name, size, vma, lma, flags in sections:
        if size == 0:
            continue
        if "ALLOC" not in flags:
            continue

        # Flash is counted by LOAD sections with load address in flash region.
        if "LOAD" in flags and in_ranges(lma, config.flash_ranges):
            flash_used += size

        # RAM is counted by allocated sections with runtime address in RAM region.
        if in_ranges(vma, config.ram_ranges):
            ram_used += size

    return BuildUsage(
        model=model, build=build, flash_used=flash_used, ram_used=ram_used
    )


def display_model_name(model: str) -> str:
    if model == "Pinecilv2":
        return "PinecilV2"
    return model


def build_row(used: int, total: int) -> str:
    return f"{pretty_bytes(used)} ({pct(used, total):.1f}%)"


def load_language_local_names(translations_dir: pathlib.Path) -> Dict[str, str]:
    language_names: Dict[str, str] = {}

    if not translations_dir.is_dir():
        return language_names

    for json_file in sorted(translations_dir.glob("translation_*.json")):
        code = json_file.stem.removeprefix("translation_")
        try:
            data = json.loads(json_file.read_text(encoding="utf-8"))
        except Exception:
            continue

        local_name = data.get("languageLocalName")
        if isinstance(local_name, str) and local_name.strip():
            language_names[code] = local_name.strip()

    return language_names


def display_build_name(build: str, language_names: Dict[str, str]) -> str:
    return language_names.get(build, build)


def generate_markdown(usages: List[BuildUsage], language_names: Dict[str, str]) -> str:
    if not usages:
        return (
            "### Firmware Space Usage Report\n\n"
            "No `.elf` files were found, so no memory usage data could be generated.\n"
        )

    by_model: Dict[str, List[BuildUsage]] = {}
    for u in usages:
        by_model.setdefault(u.model, []).append(u)

    lines: List[str] = []
    lines.append("### Firmware Space Usage Report")
    lines.append("")
    lines.append("Showing **min and max flash usage** builds per device.")
    lines.append("")

    for model in sorted(by_model.keys()):
        model_usages = sorted(by_model[model], key=lambda x: (x.flash_used, x.build))
        config = MODEL_CONFIG[model]

        selected: List[BuildUsage] = [model_usages[0]]
        if len(model_usages) > 1 and model_usages[-1].build != model_usages[0].build:
            selected.append(model_usages[-1])

        lines.append(f"#### {display_model_name(model)}")
        lines.append("")
        lines.append("| Build | Flash used | RAM used |")
        lines.append("|---|---:|---:|")

        for idx, usage in enumerate(selected):
            tag = ""
            if len(selected) > 1:
                tag = " (min flash)" if idx == 0 else " (max flash)"
            build_name = display_build_name(usage.build, language_names)
            lines.append(
                f"| `{build_name}`{tag} | {build_row(usage.flash_used, config.flash_total)} | "
                f"{build_row(usage.ram_used, config.ram_total)} |"
            )

        lines.append("")

    return "\n".join(lines).rstrip() + "\n"


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--elf-dir",
        default="source/Hexfile",
        help="Directory containing ELF files",
    )
    parser.add_argument(
        "--output",
        default="memory-usage-comment.md",
        help="Output markdown path",
    )
    parser.add_argument(
        "--translations-dir",
        default="Translations",
        help="Directory containing translation_*.json files",
    )
    args = parser.parse_args()

    elf_dir = pathlib.Path(args.elf_dir)
    elf_files = sorted(elf_dir.glob("*.elf"))

    usages: List[BuildUsage] = []
    errors: List[str] = []

    for elf_file in elf_files:
        try:
            usages.append(compute_usage(elf_file))
        except Exception as exc:  # pragma: no cover - CI utility script
            errors.append(f"- {elf_file.name}: {exc}")

    language_names = load_language_local_names(pathlib.Path(args.translations_dir))
    body = generate_markdown(usages, language_names)
    if errors:
        body += "\n### Parsing warnings\n\n"
        body += "Some files could not be parsed:\n"
        body += "\n".join(errors)
        body += "\n"

    pathlib.Path(args.output).write_text(body, encoding="utf-8")

    # Print a short summary to logs
    print(f"ELF files found: {len(elf_files)}")
    print(f"Usages parsed: {len(usages)}")
    if errors:
        print(f"Warnings: {len(errors)}")

    return 0


if __name__ == "__main__":
    sys.exit(main())
