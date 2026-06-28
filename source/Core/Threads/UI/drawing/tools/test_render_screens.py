#!/usr/bin/env python3
# Unit tests for tools/render_screens.py.
#
# Run with `python3 -m unittest tools.test_render_screens` from the
# drawing/ directory, or directly:
#     python3 tools/test_render_screens.py
#
# These tests exercise the generator's primitives in isolation, plus
# an end-to-end round-trip that re-renders every screens/*.toml and
# verifies the result matches the hand-written mono_*/ files.

from __future__ import annotations

import sys
import tempfile
import textwrap
import unittest
from pathlib import Path

# Make the generator importable when running this file directly.
HERE = Path(__file__).resolve().parent
DRAWING = HERE.parent
sys.path.insert(0, str(HERE))

import render_screens as rs  # noqa: E402

try:
    import tomllib  # Python 3.11+
except ModuleNotFoundError:  # pragma: no cover
    import tomli as tomllib  # type: ignore[no-redef]


def _load(toml_text: str) -> dict:
    return tomllib.loads(toml_text)


class ExpressionTests(unittest.TestCase):
    def test_setting_shorthand_expanded(self) -> None:
        self.assertEqual(
            rs.expand_settings("setting('Foo') > 0"),
            "getSettingValue(SettingsOptions::Foo) > 0",
        )

    def test_setting_shorthand_handles_double_quotes(self) -> None:
        self.assertEqual(
            rs.expand_settings('setting("Bar")'),
            "getSettingValue(SettingsOptions::Bar)",
        )

    def test_named_expression_substitution(self) -> None:
        doc = _load(
            textwrap.dedent(
                """
                [expressions]
                blink = "(xTaskGetTickCount() % 1000 < 300)"
                """
            )
        )
        r = rs.Renderer(doc, "96x16")
        out = r.expr("setting('CoolingTempBlink') && blink")
        self.assertIn("getSettingValue(SettingsOptions::CoolingTempBlink)", out)
        self.assertIn("(xTaskGetTickCount() % 1000 < 300)", out)


class AnchorResolutionTests(unittest.TestCase):
    def _renderer(self, anchors: dict, rotation: str = "lh") -> rs.Renderer:
        doc = {"anchors": {"96x16": anchors}}
        r = rs.Renderer(doc, "96x16")
        r.mirror_rotation = rotation if rotation == "rh" else None
        return r

    def test_inline_xy(self) -> None:
        r = self._renderer({})
        self.assertEqual(r.resolve_anchor({"x": 5, "y": 6}), (5, 6))

    def test_named_lh_rh(self) -> None:
        r = self._renderer({"a": {"lh": {"x": 0, "y": 0}, "rh": {"x": 50, "y": 0}}})
        self.assertEqual(r.resolve_anchor("a"), (0, 0))
        r.mirror_rotation = "rh"
        self.assertEqual(r.resolve_anchor("a"), (50, 0))

    def test_auto_mirror_when_rh_missing(self) -> None:
        r = self._renderer({"a": {"lh": {"x": 0, "y": 0}}})
        r.mirror_rotation = "rh"
        # 96 - 0 - 42 == 54
        self.assertEqual(r.resolve_anchor("a", mirror_width=42), (54, 0))

    def test_unknown_anchor_raises(self) -> None:
        r = self._renderer({})
        with self.assertRaises(rs.GenError):
            r.resolve_anchor("missing")


class ElementEmissionTests(unittest.TestCase):
    def _emit(self, element: dict, doc: dict | None = None) -> str:
        cb = rs.CodeBuilder()
        r = rs.Renderer(doc or {}, "96x16")
        r.emit_element(element, cb)
        return cb.text()

    def test_cursor(self) -> None:
        out = self._emit({"kind": "cursor", "at": {"x": 1, "y": 2}})
        self.assertEqual(out.strip(), "OLED::setCursor(1, 2);")

    def test_text_with_font(self) -> None:
        out = self._emit({"kind": "text", "value": '"hello"', "font": "small"})
        self.assertEqual(out.strip(), 'OLED::print("hello", FontStyle::SMALL);')

    def test_i18n(self) -> None:
        out = self._emit({"kind": "i18n", "key": "Foo", "font": "small"})
        self.assertEqual(
            out.strip(), "OLED::print(translatedString(Tr->Foo), FontStyle::SMALL);"
        )

    def test_number_with_leading_zeros(self) -> None:
        out = self._emit(
            {
                "kind": "number",
                "value": "tipTemp",
                "places": 3,
                "font": "small",
                "leading_zeros": True,
            }
        )
        self.assertEqual(
            out.strip(), "OLED::printNumber(tipTemp, 3, FontStyle::SMALL, true);"
        )

    def test_when_wraps_in_if(self) -> None:
        out = self._emit(
            {
                "kind": "deg_symbol",
                "font": "extras",
                "when": "setting('TemperatureInF')",
            }
        )
        self.assertIn("if (getSettingValue(SettingsOptions::TemperatureInF))", out)
        self.assertIn("OLED::printSymbolDeg(FontStyle::EXTRAS);", out)

    def test_only_skips_other_resolutions(self) -> None:
        out = self._emit({"kind": "refresh", "only": ["128x32"]})
        self.assertEqual(out.strip(), "")

    def test_features_emit_ifdef(self) -> None:
        out = self._emit({"kind": "refresh", "features": ["!NO_SLEEP_MODE"]})
        self.assertIn("#ifndef NO_SLEEP_MODE", out)
        self.assertIn("OLED::refresh();", out)
        self.assertIn("#endif", out)

    def test_image_auto_flip_in_rh_branch(self) -> None:
        cb = rs.CodeBuilder()
        doc = {"image_sizes": {"96x16": {"button": {"w": 42, "h": 16}}}}
        r = rs.Renderer(doc, "96x16")
        r.mirror_rotation = "rh"
        r.emit_element(
            {
                "kind": "image",
                "asset": "buttonA",
                "flip": "auto",
                "at": {"x": 0, "y": 0},
                "size": "button",
            },
            cb,
        )
        # rh branch: x mirrors to 96 - 0 - 42 = 54; asset flips to buttonAF.
        self.assertIn("buttonAF", cb.text())
        self.assertIn("54, 0, 42, 16", cb.text())


class ControlFlowTests(unittest.TestCase):
    def _emit(self, item: dict, doc: dict | None = None) -> str:
        cb = rs.CodeBuilder()
        r = rs.Renderer(doc or {}, "96x16")
        r.emit_item(item, cb)
        return cb.text()

    # ---- cond ----

    def test_cond_if_else(self) -> None:
        out = self._emit(
            {
                "kind": "cond",
                "branches": [
                    {"when": "x > 0", "do": [{"kind": "refresh"}]},
                    {"do": [{"kind": "clear_screen"}]},
                ],
            }
        )
        self.assertIn("if (x > 0)", out)
        self.assertIn("OLED::refresh()", out)
        self.assertIn("} else {", out)
        self.assertIn("OLED::clearScreen()", out)

    def test_cond_else_if_chain(self) -> None:
        out = self._emit(
            {
                "kind": "cond",
                "branches": [
                    {"when": "x == 1", "do": []},
                    {"when": "x == 2", "do": []},
                    {"do": []},
                ],
            }
        )
        self.assertIn("if (x == 1)", out)
        self.assertIn("else if (x == 2)", out)
        self.assertIn("} else {", out)

    def test_cond_first_branch_must_have_when(self) -> None:
        with self.assertRaises(rs.GenError):
            self._emit(
                {
                    "kind": "cond",
                    "branches": [{"do": [{"kind": "refresh"}]}],
                }
            )

    def test_cond_rotation_aware_anchors(self) -> None:
        # Inside `if (OLED::getRotation())` the body resolves anchors as rh.
        doc = {
            "anchors": {
                "96x16": {"a": {"lh": {"x": 0, "y": 0}, "rh": {"x": 50, "y": 0}}}
            }
        }
        out = self._emit(
            {
                "kind": "cond",
                "branches": [
                    {
                        "when": "OLED::getRotation()",
                        "do": [{"kind": "cursor", "at": "a"}],
                    },
                    {"do": [{"kind": "cursor", "at": "a"}]},
                ],
            },
            doc,
        )
        # rh side picks (50, 0); lh side picks (0, 0).
        self.assertIn("OLED::setCursor(50, 0)", out)
        self.assertIn("OLED::setCursor(0, 0)", out)

    # ---- for / switch / return / local / expression / block ----

    def test_for_loop(self) -> None:
        out = self._emit(
            {
                "kind": "for",
                "init": "int i = 0",
                "cond": "i < 3",
                "step": "i++",
                "do": [{"kind": "refresh"}],
            }
        )
        self.assertIn("for (int i = 0; i < 3; i++)", out)
        self.assertIn("OLED::refresh();", out)

    def test_switch_with_brace_and_default(self) -> None:
        out = self._emit(
            {
                "kind": "switch",
                "value": "k",
                "cases": [
                    {
                        "value": "1",
                        "comment": "one",
                        "brace": True,
                        "do": [{"kind": "clear_screen"}],
                    },
                    {"default": True},
                ],
            }
        )
        self.assertIn("switch (k) {", out)
        self.assertIn("case 1: // one", out)
        self.assertIn("OLED::clearScreen();", out)
        self.assertIn("} break;", out)
        self.assertIn("default:", out)

    def test_return_with_and_without_value(self) -> None:
        self.assertIn(
            "return true;",
            self._emit({"kind": "return", "value": "true"}),
        )
        self.assertIn(
            "return;",
            self._emit({"kind": "return"}),
        )

    def test_local_with_init_and_inline_comment(self) -> None:
        out = self._emit(
            {
                "kind": "local",
                "type": "uint64_t",
                "name": "id",
                "init": "getDeviceID()",
                "inline_comment": "device id",
            }
        )
        self.assertIn("uint64_t id = getDeviceID();", out)
        self.assertIn("// device id", out)

    def test_expression_emits_statement(self) -> None:
        out = self._emit({"kind": "expression", "expr": "x = y + 1"})
        self.assertEqual(out.strip(), "x = y + 1;")

    def test_block_wraps_in_braces(self) -> None:
        out = self._emit(
            {
                "kind": "block",
                "do": [{"kind": "refresh"}],
            }
        )
        self.assertTrue(out.lstrip().startswith("{"))
        self.assertIn("OLED::refresh();", out)
        self.assertIn("}", out)

    # ---- assign as a body item (for compound assignments) ----

    def test_assign_at_body_level(self) -> None:
        out = self._emit({"target": "x", "value": "42"})
        self.assertEqual(out.strip(), "x = 42;")

    # ---- multi-flag #if directive ----

    def test_features_with_pp_directive(self) -> None:
        out = self._emit(
            {
                "kind": "refresh",
                "features": ["#if defined(A) || defined(B)"],
            }
        )
        self.assertIn("#if defined(A) || defined(B)", out)
        self.assertIn("#endif", out)
        self.assertIn("OLED::refresh();", out)


class ShorthandTests(unittest.TestCase):
    """The compact `cursor = [x, y]` / `symbol = "X"` / etc. forms."""

    def _emit(self, item: dict, doc: dict | None = None) -> str:
        cb = rs.CodeBuilder()
        r = rs.Renderer(doc or {}, "96x16")
        r.emit_item(item, cb)
        return cb.text()

    def test_cursor_array_form(self) -> None:
        out = self._emit({"cursor": [3, 8]})
        self.assertEqual(out.strip(), "OLED::setCursor(3, 8);")

    def test_cursor_named_anchor(self) -> None:
        doc = {"anchors": {"96x16": {"a": [5, 6]}}}
        out = self._emit({"cursor": "a"}, doc)
        self.assertEqual(out.strip(), "OLED::setCursor(5, 6);")

    def test_symbol_with_default_font(self) -> None:
        doc = {"default_font": "large"}
        out = self._emit({"symbol": "LargeSymbolPlus"}, doc)
        self.assertEqual(out.strip(), "OLED::print(LargeSymbolPlus, FontStyle::LARGE);")

    def test_explicit_font_overrides_default(self) -> None:
        doc = {"default_font": "large"}
        out = self._emit({"symbol": "X", "font": "small"}, doc)
        self.assertEqual(out.strip(), "OLED::print(X, FontStyle::SMALL);")

    def test_missing_font_without_default_errors(self) -> None:
        with self.assertRaises(rs.GenError):
            self._emit({"symbol": "X"})

    def test_local_string_shorthand(self) -> None:
        out = self._emit({"local": "int x = 1 + 2"})
        self.assertEqual(out.strip(), "int x = 1 + 2;")

    def test_local_string_shorthand_no_init(self) -> None:
        out = self._emit({"local": "uint8_t flag"})
        self.assertEqual(out.strip(), "uint8_t flag;")

    def test_for_string_shorthand(self) -> None:
        out = self._emit({"for": "int i = 0; i < 3; i++", "do": [{"refresh": True}]})
        self.assertIn("for (int i = 0; i < 3; i++)", out)
        self.assertIn("OLED::refresh();", out)

    def test_for_string_shorthand_bad_format(self) -> None:
        with self.assertRaises(rs.GenError):
            self._emit({"for": "int i = 0; i++"})  # only two parts

    def test_return_string_shorthand(self) -> None:
        out = self._emit({"return": "true"})
        self.assertEqual(out.strip(), "return true;")

    def test_return_bool_shorthand(self) -> None:
        out = self._emit({"return": True})
        self.assertEqual(out.strip(), "return;")

    def test_flag_kinds(self) -> None:
        self.assertIn("OLED::refresh();", self._emit({"refresh": True}))
        self.assertIn("OLED::clearScreen();", self._emit({"clear_screen": True}))

    def test_image_short(self) -> None:
        doc = {"image_sizes": {"96x16": {"btn": [42, 16]}}}
        out = self._emit({"image": "buttonA", "at": [0, 0], "size": "btn"}, doc)
        self.assertEqual(out.strip(), "OLED::drawArea(0, 0, 42, 16, buttonA);")

    def test_call_short(self) -> None:
        out = self._emit({"call": "helper", "args": ["true"]})
        self.assertEqual(out.strip(), "helper(true);")

    def test_size_array_form(self) -> None:
        out = self._emit({"fill": [3, 0], "size": [4, 16], "value": 0})
        self.assertEqual(out.strip(), "OLED::fillArea(3, 0, 4, 16, 0);")

    def test_anchor_lh_rh_array_form(self) -> None:
        doc = {"anchors": {"96x16": {"a": {"lh": [0, 0], "rh": [50, 0]}}}}
        out = self._emit({"cursor": "a"}, doc)
        # Outside any rotation context this auto-splits into if/else.
        self.assertIn("if (OLED::getRotation())", out)
        self.assertIn("OLED::setCursor(50, 0)", out)
        self.assertIn("OLED::setCursor(0, 0)", out)

    def test_multiple_shorthand_keys_error(self) -> None:
        with self.assertRaises(rs.GenError):
            self._emit({"cursor": [0, 0], "symbol": "X"})

    def test_explicit_kind_disables_shorthand(self) -> None:
        # `kind` set explicitly: the loose `cursor` key is ignored.
        out = self._emit({"kind": "refresh", "cursor": [99, 99]})
        self.assertEqual(out.strip(), "OLED::refresh();")


class GroupTests(unittest.TestCase):
    def test_mirror_on_rotation_emits_both_orders(self) -> None:
        doc = {}
        cb = rs.CodeBuilder()
        r = rs.Renderer(doc, "96x16")
        r.emit_group(
            {
                "mirror_on_rotation": True,
                "elements": [
                    {"kind": "symbol", "value": "A", "font": "large"},
                    {"kind": "symbol", "value": "B", "font": "large"},
                ],
            },
            cb,
        )
        text = cb.text()
        self.assertIn("if (OLED::getRotation())", text)
        # The right-handed branch keeps declared order (A then B); the
        # left-handed (else) branch reverses to B then A.
        rh_idx = text.index("if (OLED::getRotation())")
        else_idx = text.index("else")
        self.assertLess(text.index("OLED::print(A,", rh_idx), else_idx)
        self.assertLess(text.index("OLED::print(B,", rh_idx), else_idx)
        self.assertLess(else_idx, text.index("OLED::print(B,", else_idx))
        self.assertLess(else_idx, text.index("OLED::print(A,", else_idx))


class FullFileTests(unittest.TestCase):
    def test_screen_block_promoted_to_function(self) -> None:
        doc = _load(
            textwrap.dedent(
                """
                [screen]
                name = "demo"
                function = "ui_demo"
                args = ["int x"]

                [[elements]]
                kind = "refresh"
                """
            )
        )
        out = rs.render_file(doc, "96x16")
        self.assertIn("void ui_demo(int x)", out)
        self.assertIn("OLED::refresh();", out)
        self.assertIn("#ifdef OLED_96x16", out)
        self.assertIn("#endif", out)

    def test_function_block_supports_explicit_signature(self) -> None:
        doc = _load(
            textwrap.dedent(
                """
                [file]
                name = "f"
                resolutions = ["96x16"]

                [[function]]
                signature = "bool foo(int x)"

                [[function.body]]
                kind = "raw"
                cpp = "return x > 0;"
                """
            )
        )
        out = rs.render_file(doc, "96x16")
        self.assertIn("bool foo(int x)", out)
        self.assertIn("return x > 0;", out)


class RegressionTests(unittest.TestCase):
    """End-to-end: re-render every TOML and verify the output is well-formed."""

    def test_all_screens_render_without_error(self) -> None:
        """Every TOML in screens/ renders successfully for both resolutions."""
        screens = DRAWING / "screens"
        self.assertTrue(screens.is_dir(), f"missing: {screens}")
        toml_files = sorted(screens.glob("*.toml"))
        self.assertGreater(len(toml_files), 0, "no TOML files found")
        with tempfile.TemporaryDirectory() as tmp:
            out_root = Path(tmp)
            for toml_path in toml_files:
                rs.render_toml(toml_path, out_root)
            # Each TOML should produce one .cpp per declared resolution.
            generated = list(out_root.glob("mono_*/*.cpp"))
            self.assertEqual(
                len(generated),
                len(toml_files) * 2,  # 96x16 and 128x32
                f"expected {len(toml_files) * 2} generated files, got {len(generated)}",
            )

    def test_generated_files_have_expected_structure(self) -> None:
        """Every generated .cpp has the OLED guard, includes, and a function body."""
        screens = DRAWING / "screens"
        with tempfile.TemporaryDirectory() as tmp:
            out_root = Path(tmp)
            for toml_path in sorted(screens.glob("*.toml")):
                rs.render_toml(toml_path, out_root)
            for gen_file in out_root.glob("mono_*/*.cpp"):
                text = gen_file.read_text()
                res = gen_file.parent.name.removeprefix("mono_")
                self.assertIn(f"#ifdef OLED_{res}", text, gen_file.name)
                self.assertIn("#endif", text, gen_file.name)
                self.assertIn("#include", text, gen_file.name)


if __name__ == "__main__":
    unittest.main()
