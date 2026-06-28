#!/usr/bin/env python3
# Generates per-resolution OLED drawing C++ source from TOML descriptors.
#
# Schema is documented in ../LAYOUT_DESIGN.md. This script intentionally
# stays small (~500 lines) and keeps no third-party dependencies; on
# Python 3.11+ it uses the standard-library `tomllib`, otherwise falls
# back to the optional `tomli` package.
#
# Usage:
#   render_screens.py --screens <dir> --output <dir> [--check]
#
# `--check` causes the script to compare each generated file against the
# corresponding hand-written file under `mono_<res>/` (whitespace- and
# comment-normalised) and exit non-zero on mismatch.

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path
from typing import Any

try:
    import tomllib  # Python 3.11+
except ModuleNotFoundError:  # pragma: no cover - exercised only on <3.11
    import tomli as tomllib  # type: ignore[no-redef]


# ── constants ────────────────────────────────────────────────────────


def _emit_pp_open(cb: CodeBuilder, features: list) -> None:
    """Emit `#ifdef`/`#ifndef`/`#if` directives for a feature list.

    Each entry is one of:
      - `"FOO"`              → `#ifdef FOO`
      - `"!FOO"`             → `#ifndef FOO`
      - `"#if defined(...)"` → emitted verbatim (allows multi-flag
                              expressions like
                              `#if defined(A) || defined(B)`).
    """
    for f in features:
        if f.startswith("#"):
            cb.line(f)
        elif f.startswith("!"):
            cb.line(f"#ifndef {f[1:]}")
        else:
            cb.line(f"#ifdef {f}")


def _emit_pp_close(cb: CodeBuilder, features: list) -> None:
    for _ in features:
        cb.line("#endif")


FONT_MAP = {
    "small": "FontStyle::SMALL",
    "large": "FontStyle::LARGE",
    "extras": "FontStyle::EXTRAS",
}


def _font(value: str) -> str:
    """Resolve a `font` field.

    Accepts the canonical names `small | large | extras` and maps them to
    `FontStyle::*`. Any other string is emitted verbatim, which lets a
    TOML pass through a function-arg variable (e.g. `font = "font"`) or
    a ternary expression like `font == FontStyle::LARGE ? ... : ...`.
    """
    return FONT_MAP.get(value, value)


SCREEN_DIMENSIONS = {
    "96x16": (96, 16),
    "128x32": (128, 32),
}


class GenError(Exception):
    """Raised when the TOML cannot be turned into valid C++."""


# ── helpers ──────────────────────────────────────────────────────────


def expand_settings(expr: str) -> str:
    """Translate the `setting('Foo')` shorthand into the long form."""
    return re.sub(
        r"setting\s*\(\s*['\"](\w+)['\"]\s*\)",
        r"getSettingValue(SettingsOptions::\1)",
        expr,
    )


class CodeBuilder:
    """Accumulates indented C++ lines."""

    def __init__(self) -> None:
        self._lines: list[str] = []
        self._indent = 0

    def line(self, s: str = "") -> None:
        if s == "":
            self._lines.append("")
        else:
            self._lines.append(("  " * self._indent) + s)

    def raw_block(self, text: str) -> None:
        # Preserve indentation of multi-line raw blocks; just prepend our
        # current indent to each line so they nest correctly.
        prefix = "  " * self._indent
        for ln in text.splitlines():
            self._lines.append((prefix + ln) if ln else "")

    def open_brace(self, lead: str) -> None:
        if lead:
            self.line(lead + " {")
        else:
            self.line("{")
        self._indent += 1

    def chain_brace(self, suffix: str) -> None:
        """Emit `} <suffix> {` while keeping the same indent depth.

        Used by `cond` to produce `} else if (...) {` and `} else {`
        chains.
        """
        self._indent -= 1
        self.line("} " + suffix.strip() + " {")
        self._indent += 1

    def close_brace(self, suffix: str = "") -> None:
        self._indent -= 1
        self.line("}" + suffix)

    def text(self) -> str:
        out = "\n".join(self._lines)
        if not out.endswith("\n"):
            out += "\n"
        return out


# ── shorthand normalisation ──────────────────────────────────────────

# Shorthand keys: when an element has no `kind` field, the first key in
# this map that's present is used to determine the kind, with the value
# moved to the named long-form field. Keys whose `field` is None are
# either flag-only (the value is just `true`) or specially handled.
SHORTHAND_KINDS: dict[str, tuple[str, str | None]] = {
    "cursor": ("cursor", "at"),
    "i18n": ("i18n", "key"),
    "text": ("text", "value"),
    "number": ("number", "value"),
    "hex": ("hex", "value"),
    "symbol": ("symbol", "value"),
    "glyph": ("glyph", "id"),
    "image": ("image", "asset"),
    "fill": ("fill", "at"),
    "deg_symbol": ("deg_symbol", "font"),
    "battery": ("battery", "level"),
    "heat": ("heat", "state"),
    "checkbox": ("checkbox", "state"),
    "call": ("call", "target"),
    "expr": ("expression", "expr"),
    "expression": ("expression", "expr"),
    "comment": ("comment", "text"),
    "local": ("local", None),  # special: parse "Type name = expr"
    "return": ("return", None),  # special: bool/str
    "clear_screen": ("clear_screen", None),  # flag
    "refresh": ("refresh", None),  # flag
    "blank": ("blank", None),  # flag
    "for": ("for", None),  # special: "init; cond; step"
    "switch": ("switch", "value"),
    "if": ("cond", None),  # special: 2-branch with `then`/`else`
    "cond": ("cond", None),  # flag (branches set separately)
    "block": ("block", None),  # flag (do set separately)
}


def _normalise_shorthand(e: dict) -> dict:
    """Convert shorthand element syntax into the long form `_emit_kind` expects.

    Examples:
        { cursor = [0, 0] }       → { kind = "cursor", at = {x=0, y=0} }
        { symbol = "X" }          → { kind = "symbol", value = "X" }
        { local = "int x = 1" }   → { kind = "local", type = "int",
                                    name = "x", init = "1" }
        { for = "i=0; i<n; i++" } → { kind = "for", init = "i=0",
                                    cond = "i<n", step = "i++" }
        { return = "v" }          → { kind = "return", value = "v" }
        { return = true }         → { kind = "return" }
        { refresh = true }        → { kind = "refresh" }
    """
    if "kind" in e:
        return e
    found_keys = [k for k in SHORTHAND_KINDS if k in e]
    if not found_keys:
        return e
    if len(found_keys) > 1:
        raise GenError(f"Element has multiple shorthand keys {found_keys!r}; pick one.")
    key = found_keys[0]
    kind, target_field = SHORTHAND_KINDS[key]
    value = e[key]
    new = {k: v for k, v in e.items() if k != key}
    new["kind"] = kind
    if target_field is not None:
        new[target_field] = value
    elif kind == "local":
        if isinstance(value, str):
            decl = value
            if "=" in decl:
                lhs, init = decl.split("=", 1)
                new["init"] = init.strip()
            else:
                lhs = decl
            tokens = lhs.strip().split()
            if len(tokens) < 2:
                raise GenError(
                    f"`local` shorthand expects 'Type name [= expr]', got {value!r}"
                )
            new["name"] = tokens[-1]
            new["type"] = " ".join(tokens[:-1])
        elif isinstance(value, dict):
            new.update(value)
        else:
            raise GenError(f"Bad `local` shorthand: {value!r}")
    elif kind == "return":
        if isinstance(value, str):
            new["value"] = value
        # else value is `true` — bare return
    elif kind == "for":
        if not isinstance(value, str):
            raise GenError(f"`for` shorthand must be a string, got {value!r}")
        parts = [p.strip() for p in value.split(";")]
        if len(parts) != 3:
            raise GenError(f"`for` shorthand expects 'init; cond; step', got {value!r}")
        new["init"], new["cond"], new["step"] = parts
    elif kind == "cond" and key == "if":
        # Two-branch `if`/`then`/`else` shorthand. The `if` value becomes
        # the first branch's `when`; the `then` and `else` arrays (built
        # from `[[X.then]]` and `[[X.else]]` table headers) become the
        # bodies of the first and (optional) second branches.
        if not isinstance(value, str):
            raise GenError(f"`if` shorthand must be a string, got {value!r}")
        branches: list[dict] = [
            {
                "when": value,
                "do": new.pop("then", []),
            }
        ]
        if "else" in new:
            branches.append({"do": new.pop("else")})
        new["branches"] = branches
    # Flag-only kinds (`refresh`, `clear_screen`, `blank`, `cond`, `block`)
    # need no extra fields — the value (typically `true`) is discarded.
    return new


def _to_xy(v):
    """Accept either {x=, y=} or [x, y] form for a 2D point."""
    if isinstance(v, list) and len(v) == 2:
        return {"x": v[0], "y": v[1]}
    return v


def _to_wh(v):
    """Accept either {w=, h=} or [w, h] form for a size."""
    if isinstance(v, list) and len(v) == 2:
        return {"w": v[0], "h": v[1]}
    return v


# ── per-resolution renderer ──────────────────────────────────────────


class Renderer:
    """Renders one TOML file for one resolution."""

    def __init__(self, doc: dict, resolution: str) -> None:
        self.doc = doc
        self.resolution = resolution
        if resolution not in SCREEN_DIMENSIONS:
            raise GenError(f"Unknown resolution: {resolution!r}")
        self.width, self.height = SCREEN_DIMENSIONS[resolution]
        self.expressions = doc.get("expressions", {})
        self.anchors = self._flatten_anchors(doc.get("anchors", {}).get(resolution, {}))
        self.widths = doc.get("widths", {}).get(resolution, {})
        self.image_sizes = doc.get("image_sizes", {}).get(resolution, {})
        # Track whether we are currently inside a mirror-on-rotation group
        # emitting the right-handed branch (so anchors mirror automatically).
        self.mirror_rotation: str | None = None
        # Cascading default-font stack — file/function/group can each push
        # a value; elements that omit `font` fall back to the top of stack.
        self._default_font_stack: list[str] = []
        file_block = doc.get("file", {}) or {}
        file_default = file_block.get("default_font") or doc.get("default_font")
        if file_default:
            self._default_font_stack.append(file_default)

    # ── anchor flattening ──

    @staticmethod
    def _is_anchor_leaf(v: Any) -> bool:
        if not isinstance(v, dict):
            return True
        keys = set(v.keys())
        return bool(keys & {"lh", "rh", "x", "y", "from"})

    @classmethod
    def _flatten_anchors(cls, d: dict, prefix: str = "") -> dict:
        out: dict = {}
        for k, v in d.items():
            full = f"{prefix}{k}"
            if isinstance(v, dict) and not cls._is_anchor_leaf(v):
                out.update(cls._flatten_anchors(v, full + "."))
            else:
                out[full] = v
        return out

    # ── expressions / sizes / anchors ──

    def expr(self, e: Any) -> str:
        if e is None:
            return ""
        if isinstance(e, bool):
            return "true" if e else "false"
        if isinstance(e, (int, float)):
            return str(e)
        if not isinstance(e, str):
            raise GenError(f"Bad expression: {e!r}")
        # Substitute named expression aliases (longest first to avoid
        # accidental partial matches).
        for name in sorted(self.expressions, key=len, reverse=True):
            value = self.expressions[name]
            e = re.sub(rf"\b{re.escape(name)}\b", f"({value})", e)
        return expand_settings(e)

    def resolve_width(self, ref: Any) -> int:
        if isinstance(ref, int):
            return ref
        if isinstance(ref, str):
            if ref in self.widths:
                return int(self.widths[ref])
            if ref in self.image_sizes:
                return int(_to_wh(self.image_sizes[ref])["w"])
            raise GenError(f"Unknown width reference: {ref!r}")
        raise GenError(f"Bad width: {ref!r}")

    def _font_of(self, e: dict) -> str:
        """Return the font for an element, falling back to the cascading default."""
        font = e.get("font")
        if font is not None:
            return _font(font)
        if self._default_font_stack:
            return _font(self._default_font_stack[-1])
        raise GenError(
            f"Element of kind={e.get('kind')!r} has no `font` and no "
            f"`default_font` is in scope."
        )

    def resolve_size(self, ref: Any) -> tuple[int, int]:
        ref = _to_wh(ref)
        if isinstance(ref, dict):
            return int(ref["w"]), int(ref["h"])
        if isinstance(ref, str):
            if ref in self.image_sizes:
                s = _to_wh(self.image_sizes[ref])
                return int(s["w"]), int(s["h"])
            if ref in self.widths:
                return int(self.widths[ref]), self.height
            raise GenError(f"Unknown size reference: {ref!r}")
        raise GenError(f"Bad size: {ref!r}")

    def resolve_anchor(
        self, ref: Any, *, mirror_width: int | None = None
    ) -> tuple[int, int]:
        """Return the (x, y) for *ref* given the current rotation context.

        `mirror_width` is the width to subtract when computing automatic
        right-handed mirrors. `None` means do not auto-mirror.
        """
        rotation = self.mirror_rotation or "lh"
        spec = ref
        if isinstance(spec, str):
            if spec not in self.anchors:
                raise GenError(f"Unknown anchor: {spec!r}")
            spec = self.anchors[spec]
        spec = _to_xy(spec)
        return self._resolve_inline(spec, rotation, mirror_width)

    def _resolve_inline(
        self, spec: Any, rotation: str, mirror_width: int | None
    ) -> tuple[int, int]:
        if not isinstance(spec, dict):
            raise GenError(f"Bad anchor: {spec!r}")
        if "lh" in spec or "rh" in spec:
            chosen = _to_xy(spec.get(rotation))
            if (
                chosen is None
                and rotation == "rh"
                and "lh" in spec
                and mirror_width is not None
            ):
                lh = _to_xy(spec["lh"])
                return (self.width - int(lh["x"]) - mirror_width, int(lh["y"]))
            if chosen is None:
                chosen = _to_xy(spec.get("lh") or spec.get("rh"))
            return int(chosen["x"]), int(chosen["y"])
        if "from" in spec:
            base = self.resolve_anchor(spec["from"], mirror_width=mirror_width)
            return (base[0] + int(spec.get("dx", 0)), base[1] + int(spec.get("dy", 0)))
        if "x" in spec and "y" in spec:
            if rotation == "rh" and mirror_width is not None:
                return (self.width - int(spec["x"]) - mirror_width, int(spec["y"]))
            return int(spec["x"]), int(spec["y"])
        raise GenError(f"Bad anchor spec: {spec!r}")

    # ── element / group emission ──

    def emit_function(self, fn: dict, cb: CodeBuilder) -> None:
        sig = fn.get("signature")
        if not sig:
            name = fn["name"]
            args = ", ".join(fn.get("args", []))
            ret = fn.get("returns", "void")
            sig = f"{ret} {name}({args})"
        df = fn.get("default_font")
        if df:
            self._default_font_stack.append(df)
        try:
            cb.open_brace(sig)
            for loc in fn.get("locals", []):
                init = loc.get("init")
                init_s = f" = {self.expr(init)}" if init is not None else ""
                cb.line(f"{loc['type']} {loc['name']}{init_s};")
            for item in fn.get("body", fn.get("elements", [])):
                self.emit_item(item, cb)
            cb.close_brace()
            cb.line()
        finally:
            if df:
                self._default_font_stack.pop()

    def emit_item(self, item: dict, cb: CodeBuilder) -> None:
        # Apply shorthand normalisation first so the rest of the pipeline
        # only ever sees the long-form `kind`-based fields.
        item = _normalise_shorthand(item)
        # Distinguish element / group / assign by presence of `kind`,
        # `elements`, `target`+`value` respectively.
        if "elements" in item:
            self.emit_group(item, cb)
            return
        if "kind" in item:
            self.emit_element(item, cb)
            return
        if "target" in item and "value" in item:
            self.emit_assign(item, cb)
            return
        raise GenError(f"Unrecognised item: {item!r}")

    def emit_assign(self, item: dict, cb: CodeBuilder) -> None:
        lines = [f"{item['target']} = {self.expr(item['value'])};"]
        when = item.get("when")
        if when:
            cb.open_brace(f"if ({self.expr(when)})")
            for ln in lines:
                cb.line(ln)
            cb.close_brace()
        else:
            for ln in lines:
                cb.line(ln)

    def emit_group(self, group: dict, cb: CodeBuilder) -> None:
        only = group.get("only")
        if only and self.resolution not in only:
            return
        feats = group.get("features", []) or []
        _emit_pp_open(cb, feats)
        df = group.get("default_font")
        if df:
            self._default_font_stack.append(df)
        try:
            when = group.get("when")
            mirror = group.get("mirror_on_rotation", False)
            elements = group.get("elements", [])

            def emit_inner(rotation: str) -> None:
                seq = list(elements)
                if rotation == "lh" and mirror:
                    seq = list(reversed(seq))
                self.mirror_rotation = rotation if mirror else None
                try:
                    for el in seq:
                        self.emit_item(el, cb)
                finally:
                    self.mirror_rotation = None

            if mirror:
                # `mirror_on_rotation` produces an if/else over OLED::getRotation().
                # The declared element order corresponds to the right-handed
                # branch (matching how the original C++ for soldering_basic_status
                # is read top-to-bottom).
                outer_when = self.expr(when) if when else None
                if outer_when is not None:
                    cb.open_brace(f"if ({outer_when})")
                cb.open_brace("if (OLED::getRotation())")
                emit_inner("rh")
                cb.close_brace(" else {")
                cb._indent += 1  # type: ignore[attr-defined]
                emit_inner("lh")
                cb._indent -= 1  # type: ignore[attr-defined]
                cb.line("}")
                if outer_when is not None:
                    cb.close_brace()
            else:
                if when:
                    cb.open_brace(f"if ({self.expr(when)})")
                for el in elements:
                    self.emit_item(el, cb)
                if when:
                    cb.close_brace()
        finally:
            if df:
                self._default_font_stack.pop()

        _emit_pp_close(cb, feats)

    def emit_element(self, e: dict, cb: CodeBuilder) -> None:
        only = e.get("only")
        if only and self.resolution not in only:
            return
        feats = e.get("features", []) or []
        _emit_pp_open(cb, feats)

        when = e.get("when")
        if when:
            cb.open_brace(f"if ({self.expr(when)})")
        try:
            comment = e.get("comment")
            if comment:
                cb.line(
                    comment if comment.lstrip().startswith("//") else f"// {comment}"
                )
            # When `at` carries both lh and rh anchors and we are NOT inside
            # a mirror group, auto-expand to a runtime if/else over rotation.
            if self._needs_rotation_split(e):
                self._emit_rotation_split(e, cb)
            else:
                self._emit_kind(e, cb)
        finally:
            if when:
                cb.close_brace()
            _emit_pp_close(cb, feats)

    def _needs_rotation_split(self, e: dict) -> bool:
        if self.mirror_rotation is not None:
            return False
        # Only kinds that read `at` need the split.
        if e.get("kind") not in {"cursor", "image", "fill", "call"}:
            return False
        at = e.get("at")
        if isinstance(at, dict):
            return "lh" in at and "rh" in at
        if isinstance(at, str):
            spec = self.anchors.get(at)
            if isinstance(spec, dict):
                return "lh" in spec and "rh" in spec
        # `flip = auto` images always need a rotation split, even if the
        # `at` doesn't carry rotation info — the asset name flips.
        if e.get("kind") == "image" and e.get("flip") == "auto":
            return True
        return False

    def _emit_rotation_split(self, e: dict, cb: CodeBuilder) -> None:
        cb.open_brace("if (OLED::getRotation())")
        self.mirror_rotation = "rh"
        try:
            self._emit_kind(e, cb)
        finally:
            self.mirror_rotation = None
        cb.chain_brace("else")
        self.mirror_rotation = "lh"
        try:
            self._emit_kind(e, cb)
        finally:
            self.mirror_rotation = None
        cb.close_brace()

    def _emit_kind(self, e: dict, cb: CodeBuilder) -> None:  # noqa: C901
        kind = e["kind"]

        if kind == "raw":
            cb.raw_block(e["cpp"].rstrip("\n"))
            return

        if kind == "clear_screen":
            cb.line("OLED::clearScreen();")
            return

        if kind == "refresh":
            cb.line("OLED::refresh();")
            return

        if kind == "cursor":
            mw = e.get("mirror_width")
            mw_int = self.resolve_width(mw) if mw is not None else None
            x, y = self.resolve_anchor(e["at"], mirror_width=mw_int)
            cb.line(f"OLED::setCursor({x}, {y});")
            return

        if kind == "text":
            font = self._font_of(e)
            value = self.expr(e["value"])
            extras = []
            if "length" in e:
                extras.append(str(e["length"]))
                if "soft_x_limit" in e:
                    extras.append(str(e["soft_x_limit"]))
            extras_s = (", " + ", ".join(extras)) if extras else ""
            cb.line(f"OLED::print({value}, {font}{extras_s});")
            return

        if kind == "i18n":
            font = self._font_of(e)
            cb.line(f"OLED::print(translatedString(Tr->{e['key']}), {font});")
            return

        if kind == "number":
            font = self._font_of(e)
            v = self.expr(e["value"])
            places = e.get("places", 1)
            args = [v, str(places), font]
            if "leading_zeros" in e:
                args.append("true" if e["leading_zeros"] else "false")
            cb.line(f"OLED::printNumber({', '.join(args)});")
            return

        if kind == "hex":
            font = self._font_of(e)
            v = self.expr(e["value"])
            digits = e.get("digits", 8)
            cb.line(f"OLED::drawHex({v}, {font}, {digits});")
            return

        if kind == "symbol":
            font = self._font_of(e)
            cb.line(f"OLED::print({self.expr(e['value'])}, {font});")
            return

        if kind == "glyph":
            cb.line(f"OLED::drawSymbol({e['id']});")
            return

        if kind == "deg_symbol":
            font = self._font_of(e)
            cb.line(f"OLED::printSymbolDeg({font});")
            return

        if kind == "image":
            asset = e["asset"]
            w, h = self.resolve_size(e["size"])
            x, y = self.resolve_anchor(e["at"], mirror_width=w)
            if e.get("flip") == "auto" and self.mirror_rotation == "rh":
                # Inside a mirror group, RH branch uses the pre-flipped buffer.
                asset = asset + "F"
            cb.line(f"OLED::drawArea({x}, {y}, {w}, {h}, {asset});")
            return

        if kind == "fill":
            w, h = self.resolve_size(e["size"])
            x, y = self.resolve_anchor(e["at"], mirror_width=w)
            value = e.get("value", 0)
            cb.line(f"OLED::fillArea({x}, {y}, {w}, {h}, {value});")
            return

        if kind == "rect":
            clear = "true" if e.get("clear", False) else "false"
            cb.line(
                f"OLED::drawFilledRect({e['x0']}, {e['y0']}, "
                f"{e['x1']}, {e['y1']}, {clear});"
            )
            return

        if kind == "battery":
            cb.line(f"OLED::drawBattery({self.expr(e['level'])});")
            return

        if kind == "heat":
            cb.line(f"OLED::drawHeatSymbol({self.expr(e['state'])});")
            return

        if kind == "checkbox":
            cb.line(f"OLED::drawCheckbox({self.expr(e['state'])});")
            return

        if kind == "call":
            target = e["target"]
            args = ", ".join(self.expr(a) for a in e.get("args", []))
            if "at" in e:
                mw = e.get("width")
                mw_int = self.resolve_width(mw) if mw is not None else None
                x, y = self.resolve_anchor(e["at"], mirror_width=mw_int)
                cb.line(f"OLED::setCursor({x}, {y});")
            cb.line(f"{target}({args});")
            return

        if kind == "cond":
            self._emit_cond(e, cb)
            return

        if kind == "for":
            init = e.get("init", "")
            cond = e.get("cond", "")
            step = e.get("step", "")
            cb.open_brace(f"for ({init}; {cond}; {step})")
            for sub in e.get("do", []):
                self.emit_item(sub, cb)
            cb.close_brace()
            return

        if kind == "switch":
            self._emit_switch(e, cb)
            return

        if kind == "return":
            value = e.get("value")
            if value is None:
                cb.line("return;")
            else:
                cb.line(f"return {self.expr(value)};")
            return

        if kind == "local":
            init = e.get("init")
            init_s = f" = {self.expr(init)}" if init is not None else ""
            inline = ""
            if e.get("inline_comment"):
                inline = f" // {e['inline_comment']}"
            cb.line(f"{e['type']} {e['name']}{init_s};{inline}")
            return

        if kind == "expression":
            cb.line(f"{self.expr(e['expr'])};")
            return

        if kind == "block":
            cb.open_brace("")
            for sub in e.get("do", []):
                self.emit_item(sub, cb)
            cb.close_brace()
            return

        if kind == "comment":
            text = e["text"]
            if not text.lstrip().startswith(("//", "/*")):
                text = "// " + text
            cb.line(text)
            return

        if kind == "blank":
            cb.line()
            return

        raise GenError(f"Unknown element kind: {kind!r}")

    def _emit_cond(self, e: dict, cb: CodeBuilder) -> None:
        """Emit a chain of `if`/`else if`/`else` branches.

        As a convenience: branches whose `when` is exactly `OLED::getRotation()`
        or `!OLED::getRotation()` cause anchor resolution inside that branch
        to pick the `rh` (or `lh`) variant of any `{ lh = ..., rh = ... }`
        anchor it touches. Without this, every rotation-gated cond would
        need to repeat both halves of every anchor.
        """
        branches = e.get("branches", [])
        if not branches:
            return
        first = branches[0]
        when = first.get("when")
        if not when:
            raise GenError("First branch of `cond` must have a `when`.")
        cb.open_brace(f"if ({self.expr(when)})")
        self._emit_cond_branch(first, cb, when)
        for branch in branches[1:]:
            w = branch.get("when")
            if w is not None:
                cb.chain_brace(f"else if ({self.expr(w)})")
            else:
                cb.chain_brace("else")
            self._emit_cond_branch(branch, cb, w, prev_when=when)
            if w is not None:
                # `when` for an `else if` becomes the new prev_when chain.
                when = w
        cb.close_brace()

    def _emit_cond_branch(
        self,
        branch: dict,
        cb: CodeBuilder,
        when: str | None,
        prev_when: str | None = None,
    ) -> None:
        rotation = self._rotation_from_when(when)
        if rotation is None and when is None and prev_when is not None:
            # `else` after a getRotation() branch — the inverse rotation.
            inverse = self._rotation_from_when(prev_when)
            if inverse == "rh":
                rotation = "lh"
            elif inverse == "lh":
                rotation = "rh"
        prev = self.mirror_rotation
        if rotation is not None:
            self.mirror_rotation = rotation
        try:
            for sub in branch.get("do", []):
                self.emit_item(sub, cb)
        finally:
            self.mirror_rotation = prev

    @staticmethod
    def _rotation_from_when(when_expr: str | None) -> str | None:
        if when_expr is None:
            return None
        s = "".join(when_expr.split())
        if s == "OLED::getRotation()":
            return "rh"
        if s == "!OLED::getRotation()":
            return "lh"
        return None

    def _emit_switch(self, e: dict, cb: CodeBuilder) -> None:
        """Emit a `switch` statement.

        Each case may carry:
          value      — the case label expression (omit for `default`)
          default    — set to true to make this the `default:` branch
          body       — list of items inside the case
          brace      — wrap body in `{ ... }` (needed for case-local vars)
          no_break   — omit the trailing `break;`
          comment    — trailing `// ...` after the `case N:` label
          features   — #ifdef gates around the entire case
        """
        value = self.expr(e["value"])
        cb.open_brace(f"switch ({value})")
        for case in e.get("cases", []):
            feats = case.get("features", []) or []
            _emit_pp_open(cb, feats)
            label = "default:" if case.get("default") else f"case {case['value']}:"
            comment = case.get("comment")
            if comment:
                cb.line(f"{label} // {comment}")
            else:
                cb.line(label)
            no_break = case.get("no_break", False)
            if case.get("brace", False):
                cb.line("{")
                cb._indent += 1  # type: ignore[attr-defined]
                for sub in case.get("do", []):
                    self.emit_item(sub, cb)
                cb._indent -= 1  # type: ignore[attr-defined]
                if no_break:
                    cb.line("}")
                else:
                    cb.line("} break;")
            else:
                cb._indent += 1  # type: ignore[attr-defined]
                for sub in case.get("do", []):
                    self.emit_item(sub, cb)
                if not no_break:
                    cb.line("break;")
                cb._indent -= 1  # type: ignore[attr-defined]
            _emit_pp_close(cb, feats)
        cb.close_brace()


# ── file-level orchestration ─────────────────────────────────────────


def render_file(doc: dict, resolution: str) -> str:
    cb = CodeBuilder()
    file_block = doc.get("file", {}) or {}
    cb.line("// Auto-generated by tools/render_screens.py — do not edit.")
    cb.line("// Edit the corresponding .toml file in screens/ instead.")
    includes = doc.get("includes") or file_block.get("includes") or []
    if includes:
        for inc in includes:
            if inc.startswith("<") and inc.endswith(">"):
                cb.line(f"#include {inc}")
            else:
                cb.line(f'#include "{inc}"')
    else:
        cb.line('#include "ui_drawing.hpp"')
    cb.line()
    cb.line(f"#ifdef OLED_{resolution}")
    cb.line()
    declarations = doc.get("declarations") or file_block.get("declarations") or []
    for decl in declarations:
        cb.raw_block(decl)
        cb.line()
    if "prelude" in doc:
        cb.raw_block(doc["prelude"])
        cb.line()
    renderer = Renderer(doc, resolution)
    functions = doc.get("functions") or doc.get("function") or []
    if isinstance(functions, dict):
        functions = [functions]
    if not functions and "screen" in doc:
        # Single-screen shorthand — promote to a function entry.
        screen = doc["screen"]
        fn = {
            "name": screen["function"],
            "args": screen.get("args", []),
            "body": doc.get("elements", []),
            "locals": doc.get("locals", []),
        }
        functions = [fn]
    for fn in functions:
        # Allow per-function `features` wrappers (e.g. NO_SLEEP_MODE).
        feats = fn.get("features", []) or []
        _emit_pp_open(cb, feats)
        renderer.emit_function(fn, cb)
        _emit_pp_close(cb, feats)
    cb.line("#endif")
    return cb.text()


def render_toml(path: Path, output_root: Path) -> list[Path]:
    with path.open("rb") as fp:
        doc = tomllib.load(fp)
    file_block = doc.get("file") or {}
    name = file_block.get("name")
    if not name:
        # Derive from `[screen].name` or filename.
        name = doc.get("screen", {}).get("name") or path.stem
    resolutions = (
        file_block.get("resolutions")
        or doc.get("screen", {}).get("resolutions")
        or list(SCREEN_DIMENSIONS.keys())
    )
    written: list[Path] = []
    for res in resolutions:
        out_dir = output_root / f"mono_{res}"
        out_dir.mkdir(parents=True, exist_ok=True)
        out_path = out_dir / f"{name}.cpp"
        out_path.write_text(render_file(doc, res))
        written.append(out_path)
    return written


# ── normalised diff for `--check` mode ───────────────────────────────


_COMMENT_RE = re.compile(r"//.*?$|/\*.*?\*/", re.MULTILINE | re.DOTALL)
_WS_RE = re.compile(r"\s+")


def normalise_cpp(text: str) -> str:
    text = _COMMENT_RE.sub(" ", text)
    text = _WS_RE.sub(" ", text)
    return text.strip()


def check_against_existing(
    generated_root: Path, reference_root: Path
) -> list[tuple[Path, Path]]:
    """Return list of (generated, reference) pairs that differ."""
    diffs: list[tuple[Path, Path]] = []
    for gen_dir in sorted(generated_root.glob("mono_*")):
        ref_dir = reference_root / gen_dir.name
        if not ref_dir.is_dir():
            continue
        for gen_file in sorted(gen_dir.glob("*.cpp")):
            ref_file = ref_dir / gen_file.name
            if not ref_file.exists():
                continue
            if normalise_cpp(gen_file.read_text()) != normalise_cpp(
                ref_file.read_text()
            ):
                diffs.append((gen_file, ref_file))
    return diffs


def main(argv: list[str] | None = None) -> int:
    here = Path(__file__).resolve().parent.parent
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument(
        "--screens",
        type=Path,
        default=here / "screens",
        help="Directory containing TOML descriptors.",
    )
    ap.add_argument(
        "--output",
        type=Path,
        default=here / "generated",
        help="Where to write generated .cpp files.",
    )
    ap.add_argument(
        "--check",
        type=Path,
        default=None,
        help=(
            "If set, compare each generated file against the same-named "
            "file under <check>/mono_<res>/ (whitespace/comment normalised)."
        ),
    )
    args = ap.parse_args(argv)

    if not args.screens.is_dir():
        print(f"Screens directory not found: {args.screens}", file=sys.stderr)
        return 2

    written: list[Path] = []
    for toml_path in sorted(args.screens.glob("*.toml")):
        try:
            written.extend(render_toml(toml_path, args.output))
        except GenError as exc:
            print(f"{toml_path}: {exc}", file=sys.stderr)
            return 1

    print(f"Generated {len(written)} file(s) under {args.output}")

    if args.check is not None:
        diffs = check_against_existing(args.output, args.check)
        if diffs:
            for gen, ref in diffs:
                print(f"DIFF: {gen} vs {ref}", file=sys.stderr)
            return 1
        print(f"All generated files match references under {args.check}.")

    return 0


if __name__ == "__main__":
    sys.exit(main())
