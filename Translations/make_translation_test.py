#!/usr/bin/env python3
import unittest


class TestMakeTranslation(unittest.TestCase):
    def test_get_bytes_from_font_index(self):
        from make_translation import get_bytes_from_font_index

        self.assertEqual(get_bytes_from_font_index(2), b"\x02")
        self.assertEqual(get_bytes_from_font_index(239), b"\xEF")
        self.assertEqual(get_bytes_from_font_index(240), b"\xF0")
        self.assertEqual(get_bytes_from_font_index(241), b"\xF1\x01")
        self.assertEqual(get_bytes_from_font_index(495), b"\xF1\xFF")
        self.assertEqual(get_bytes_from_font_index(496), b"\xF2\x01")
        self.assertEqual(get_bytes_from_font_index(750), b"\xF2\xFF")
        self.assertEqual(get_bytes_from_font_index(751), b"\xF3\x01")
        self.assertEqual(get_bytes_from_font_index(0x10 * 0xFF - 15), b"\xFF\xFF")
        with self.assertRaises(ValueError):
            get_bytes_from_font_index(0x10 * 0xFF - 14)

    def test_bytes_to_escaped(self):
        from make_translation import bytes_to_escaped

        self.assertEqual(bytes_to_escaped(b"\x00"), "\\x00")
        self.assertEqual(bytes_to_escaped(b"\xF1\xAB"), "\\xF1\\xAB")

    def test_bytes_to_c_hex(self):
        from make_translation import bytes_to_c_hex

        self.assertEqual(bytes_to_c_hex(b"\x00"), "0x00,")
        self.assertEqual(bytes_to_c_hex(b"\xF1\xAB"), "0xF1, 0xAB,")


if __name__ == "__main__":
    unittest.main()
