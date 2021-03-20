import unittest


class TestMakeTranslation(unittest.TestCase):
    def test_getCharsFromFontIndex(self):
        from make_translation import getCharsFromFontIndex
        self.assertEqual(getCharsFromFontIndex(2), "\\x02")
        self.assertEqual(getCharsFromFontIndex(239), "\\xEF")
        self.assertEqual(getCharsFromFontIndex(240), "\\xF0")
        self.assertEqual(getCharsFromFontIndex(241), "\\xF1\\x01")
        self.assertEqual(getCharsFromFontIndex(495), "\\xF1\\xFF")
        self.assertEqual(getCharsFromFontIndex(496), "\\xF2\\x01")
        self.assertEqual(getCharsFromFontIndex(750), "\\xF2\\xFF")
        self.assertEqual(getCharsFromFontIndex(751), "\\xF3\\x01")
        self.assertEqual(getCharsFromFontIndex(0x10 * 0xFF - 15), "\\xFF\\xFF")
        with self.assertRaises(AssertionError):
            getCharsFromFontIndex(0x10 * 0xFF - 14)


if __name__ == '__main__':
    unittest.main()
