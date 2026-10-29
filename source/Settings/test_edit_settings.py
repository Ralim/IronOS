#! python3
import unittest
import os
import sys
import tempfile
import struct
from pathlib import Path

# Add parent directory to path to import edit_settings module
sys.path.insert(0, str(Path(__file__).parent))
from edit_settings import SettingsEntry, Settings


class TestSettingsEntry(unittest.TestCase):
    def test_settings_entry_init(self):
        """Test SettingsEntry initialization"""
        entry = SettingsEntry(10, 100, 5, 20, "TestSetting")
        self.assertEqual(entry.min, 10)
        self.assertEqual(entry.max, 100)
        self.assertEqual(entry.increment, 5)
        self.assertEqual(entry.default, 20)
        self.assertEqual(entry.name, "TestSetting")

    def test_settings_entry_str(self):
        """Test SettingsEntry string representation"""
        entry = SettingsEntry(10, 100, 5, 20, "TestSetting")
        self.assertIn("TestSetting", str(entry))
        self.assertIn("20", str(entry))
        self.assertIn("10", str(entry))
        self.assertIn("100", str(entry))
        self.assertIn("5", str(entry))


class TestSettings(unittest.TestCase):
    def setUp(self):
        """Set up test fixtures"""
        self.settings = Settings()

        # Create a temporary YAML file for testing
        self.temp_yaml = tempfile.NamedTemporaryFile(
            delete=False, suffix=".yaml", mode="w"
        )
        self.temp_yaml.write(
            """
settings:
  - default: 20
    increment: 5
    max: 100
    min: 10
    name: Setting1
  - default: 500
    increment: 10
    max: 1000
    min: 0
    name: Setting2
  - default: 1
    increment: 1
    max: 1
    min: 0
    name: Setting3
"""
        )
        self.temp_yaml.close()

        # Create a temporary binary file for testing
        self.temp_binary = tempfile.NamedTemporaryFile(
            delete=False, suffix=".bin", mode="wb"
        )
        # Write three uint16_t values in little-endian: 30, 600, 0
        self.temp_binary.write(struct.pack("<HHH", 30, 600, 0))
        self.temp_binary.close()

        # Output file path for testing save operations
        self.output_binary = tempfile.NamedTemporaryFile(
            delete=False, suffix=".bin"
        ).name

    def tearDown(self):
        """Tear down test fixtures"""
        os.unlink(self.temp_yaml.name)
        os.unlink(self.temp_binary.name)
        if os.path.exists(self.output_binary):
            os.unlink(self.output_binary)

    def test_load_from_yaml(self):
        """Test loading settings from YAML file"""
        self.settings.load_from_yaml(self.temp_yaml.name)

        # Check that entries were loaded correctly
        self.assertEqual(len(self.settings.entries), 3)
        self.assertEqual(self.settings.entries[0].name, "Setting1")
        self.assertEqual(self.settings.entries[0].default, 20)
        self.assertEqual(self.settings.entries[1].name, "Setting2")
        self.assertEqual(self.settings.entries[1].min, 0)
        self.assertEqual(self.settings.entries[2].name, "Setting3")
        self.assertEqual(self.settings.entries[2].max, 1)

        # Check that values were set to defaults
        self.assertEqual(len(self.settings.values), 3)
        self.assertEqual(self.settings.values[0], 20)
        self.assertEqual(self.settings.values[1], 500)
        self.assertEqual(self.settings.values[2], 1)

    def test_load_from_binary(self):
        """Test loading settings from binary file"""
        # First load YAML to set up entries
        self.settings.load_from_yaml(self.temp_yaml.name)

        # Then load binary values
        result = self.settings.load_from_binary(self.temp_binary.name)

        # Check result
        self.assertTrue(result)

        # Check that values were updated from binary
        self.assertEqual(self.settings.values[0], 30)  # Changed from default 20
        self.assertEqual(self.settings.values[1], 600)  # Changed from default 500
        self.assertEqual(self.settings.values[2], 0)  # Changed from default 1

    def test_load_from_nonexistent_binary(self):
        """Test loading settings from a nonexistent binary file"""
        self.settings.load_from_yaml(self.temp_yaml.name)
        result = self.settings.load_from_binary("nonexistent_file.bin")
        self.assertFalse(result)

        # Values should still be defaults
        self.assertEqual(self.settings.values[0], 20)
        self.assertEqual(self.settings.values[1], 500)
        self.assertEqual(self.settings.values[2], 1)

    def test_save_to_binary(self):
        """Test saving settings to binary file"""
        # Set up settings
        self.settings.load_from_yaml(self.temp_yaml.name)
        self.settings.values = [25, 550, 1]  # Custom values

        # Save to binary
        result = self.settings.save_to_binary(self.output_binary)
        self.assertTrue(result)

        # Check the file exists
        self.assertTrue(os.path.exists(self.output_binary))

        # Read the binary data and verify
        with open(self.output_binary, "rb") as f:
            data = f.read()

        self.assertEqual(len(data), 6)  # 3 settings * 2 bytes
        values = struct.unpack("<HHH", data)
        self.assertEqual(values[0], 25)
        self.assertEqual(values[1], 550)
        self.assertEqual(values[2], 1)

    def test_save_to_binary_error_handling(self):
        """Test error handling when saving to an invalid location"""
        self.settings.load_from_yaml(self.temp_yaml.name)
        # Try to save to a location that doesn't exist
        result = self.settings.save_to_binary("/nonexistent/directory/file.bin")
        self.assertFalse(result)


class TestSettingsEditingFunctional(unittest.TestCase):
    """
    These tests would normally involve mocking user input to test edit_all_settings.
    However, since mocking stdin is complex and potentially brittle, we'll skip those tests.
    In a real project, you might use a library like unittest.mock to patch input() or
    restructure the code to allow for dependency injection of user input.
    """

    pass


if __name__ == "__main__":
    unittest.main()
