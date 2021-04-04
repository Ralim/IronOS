#!/usr/bin/env python3
import unittest


class TestMakeTranslation(unittest.TestCase):
    def test_get_chars_from_font_index(self):
        from make_translation import get_chars_from_font_index

        self.assertEqual(get_chars_from_font_index(2), "\\x02")
        self.assertEqual(get_chars_from_font_index(239), "\\xEF")
        self.assertEqual(get_chars_from_font_index(240), "\\xF0")
        self.assertEqual(get_chars_from_font_index(241), "\\xF1\\x01")
        self.assertEqual(get_chars_from_font_index(495), "\\xF1\\xFF")
        self.assertEqual(get_chars_from_font_index(496), "\\xF2\\x01")
        self.assertEqual(get_chars_from_font_index(750), "\\xF2\\xFF")
        self.assertEqual(get_chars_from_font_index(751), "\\xF3\\x01")
        self.assertEqual(get_chars_from_font_index(0x10 * 0xFF - 15), "\\xFF\\xFF")
        with self.assertRaises(ValueError):
            get_chars_from_font_index(0x10 * 0xFF - 14)


if __name__ == "__main__":
    unittest.main()
