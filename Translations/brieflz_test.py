#!/usr/bin/env python3
import brieflz
import unittest


TEST_DATA = (
    b"Lorem ipsum dolor sit amet, consectetur adipiscing elit. "
    b"Ut consequat mattis orci ac laoreet. Duis ac turpis tempus, varius lacus non, dignissim lectus. "
    b"Curabitur quis metus luctus, sollicitudin ipsum at, dictum metus. "
    b"Cras sed est nec ex tempor tincidunt in at ante. Vivamus laoreet urna eget lectus euismod feugiat. "
    b"Duis a massa ac metus pellentesque interdum. Nunc congue, est faucibus convallis commodo, justo nibh sagittis augue, sed tristique urna neque vitae urna. "
    b"Donec quis orci et purus imperdiet sollicitudin."
)


class TestBriefLZ(unittest.TestCase):
    def test_roundtrip(self):
        packed = brieflz.compress(TEST_DATA)
        depacked = brieflz.depack_srcsize(packed, len(TEST_DATA))
        self.assertEqual(depacked, TEST_DATA)


if __name__ == "__main__":
    unittest.main()
