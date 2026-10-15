#!/usr/bin/env python3
import io
import json
import os
import unittest


class TestMakeTranslation(unittest.TestCase):
    def test_optional_worklight_uses_localized_or_default_text(self):
        import make_translation as translation

        defs = translation.load_json(translation.HERE / "translations_definitions.json")
        record = next(
            item for item in defs["menuOptions"] if item["id"] == "WorklightEnabled"
        )
        for filename in translation.HERE.glob("translation_*.json"):
            for enabled in (False, True):
                lang = translation.load_json(filename)
                expected = (
                    lang["menuOptions"]
                    .get("WorklightEnabled", record["default"])
                    .copy()
                )
                macros = frozenset(("WORKLIGHT_SETTING",)) if enabled else frozenset()
                with self.subTest(language=lang["languageCode"], worklight=enabled):
                    result = translation.filter_translation(lang, defs, macros)
                    self.assertEqual(
                        result["menuOptions"]["WorklightEnabled"],
                        expected if enabled else {"displayText": "", "description": ""},
                    )
        self.assertEqual(record["default"]["displayText"], "Work\nlight")

    def test_single_language_combines_font_and_string_compression(self):
        import make_translation as translation

        defs = translation.load_json(translation.HERE / "translations_definitions.json")
        lang = translation.filter_translation(
            translation.read_translation(translation.HERE, "EN"),
            defs,
            frozenset(("WORKLIGHT_SETTING",)),
        )
        data = translation.prepare_language(lang, defs, "test")
        output = io.StringIO()
        raw = b"\x00\x01\x02worklight" * 32
        translation.write_language(data, output, strings_bin=raw, compress_font=True)
        source = output.getvalue()
        preparation = source.split("void prepareTranslations() {", 1)[1].split("}", 1)[
            0
        ]
        for name, buffer in (
            ("font_12x16_brieflz", "font12_out_buffer"),
            ("font_06x08_brieflz", "font06_out_buffer"),
            ("translation_data_brieflz", "translation_data_out_buffer"),
        ):
            self.assertIn(f"static uint8_t {buffer}[", source)
            self.assertIn(
                f"blz_depack_srcsize({name}, {buffer}, sizeof({name}));", preparation
            )

    def test_heater_stop_warning_is_localized_and_variant_specific(self):
        import make_translation as translation

        defs = translation.load_json(translation.HERE / "translations_definitions.json")
        for filename in translation.HERE.glob("translation_*.json"):
            for heater_test in (False, True):
                lang = translation.load_json(filename)
                original = lang["messagesWarn"]["WarningThermalRunaway"]["message"]
                expected = lang["messagesWarn"].get(
                    "WarningHeaterStopped", {"message": "Heating stopped\n"}
                )["message"]
                macros = (
                    frozenset(("TS21_HEATER_TEST",)) if heater_test else frozenset()
                )
                with self.subTest(language=lang["languageCode"], heater=heater_test):
                    result = translation.filter_translation(lang, defs, macros)
                    self.assertEqual(
                        result["messagesWarn"]["WarningHeaterStopped"]["message"],
                        expected if heater_test else "",
                    )
                    self.assertEqual(
                        result["messagesWarn"]["WarningThermalRunaway"]["message"],
                        "" if heater_test else original,
                    )

    def test_single_language_initializes_both_compressed_fonts(self):
        import make_translation as translation

        defs = translation.load_json(translation.HERE / "translations_definitions.json")
        lang = translation.filter_translation(
            translation.read_translation(translation.HERE, "EN"), defs, frozenset()
        )
        data = translation.prepare_language(lang, defs, "test")
        output = io.StringIO()
        translation.write_language(data, output, compress_font=True)
        source = output.getvalue()
        preparation = source.split("void prepareTranslations() {", 1)[1].split("}", 1)[
            0
        ]
        for name, buffer in (
            ("font_12x16_brieflz", "font12_out_buffer"),
            ("font_06x08_brieflz", "font06_out_buffer"),
        ):
            self.assertIn(f"static uint8_t {buffer}[", source)
            self.assertIn(
                f"blz_depack_srcsize({name}, {buffer}, sizeof({name}));",
                preparation,
            )
        self.assertNotIn("font_out_buffer", source)

    def test_get_bytes_from_font_index(self):
        from make_translation import get_bytes_from_font_index

        self.assertEqual(get_bytes_from_font_index(2), b"\x02")
        self.assertEqual(get_bytes_from_font_index(239), b"\xef")
        self.assertEqual(get_bytes_from_font_index(240), b"\xf0")
        self.assertEqual(get_bytes_from_font_index(241), b"\xf1\x01")
        self.assertEqual(get_bytes_from_font_index(495), b"\xf1\xff")
        self.assertEqual(get_bytes_from_font_index(496), b"\xf2\x01")
        self.assertEqual(get_bytes_from_font_index(750), b"\xf2\xff")
        self.assertEqual(get_bytes_from_font_index(751), b"\xf3\x01")
        self.assertEqual(get_bytes_from_font_index(0x10 * 0xFF - 15), b"\xff\xff")
        with self.assertRaises(ValueError):
            get_bytes_from_font_index(0x10 * 0xFF - 14)

    def test_bytes_to_escaped(self):
        from make_translation import bytes_to_escaped

        self.assertEqual(bytes_to_escaped(b"\x00"), "\\x00")
        self.assertEqual(bytes_to_escaped(b"\xf1\xab"), "\\xF1\\xAB")

    def test_bytes_to_c_hex(self):
        from make_translation import bytes_to_c_hex

        self.assertEqual(bytes_to_c_hex(b"\x00"), "0x00,")
        self.assertEqual(bytes_to_c_hex(b"\xf1\xab"), "0xF1, 0xAB,")

    def test_no_language_id_collisions(self):
        """
        Asserting that we have no language collisions and that the has works ok
        """
        from make_translation import get_language_unqiue_id

        seen_ids = []
        for filename in os.listdir("."):
            if filename.endswith(".json") and filename.startswith("translation_"):
                with open(filename) as f:
                    data = json.loads(f.read())
                    lang_code = data.get("languageCode")
                    self.assertNotEqual(lang_code, None)
                    id = get_language_unqiue_id(lang_code)
                    self.assertFalse(id in seen_ids)
                    seen_ids.append(id)


if __name__ == "__main__":
    unittest.main()
