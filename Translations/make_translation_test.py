#!/usr/bin/env python3
import json
import os
import unittest


class TestMakeTranslation(unittest.TestCase):
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
