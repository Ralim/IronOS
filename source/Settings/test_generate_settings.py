#!/usr/bin/env python3
"""
Test script for generate_settings.py

This script tests the functionality of the settings generator
by creating a temporary YAML file and comparing the output with expected results.
"""

import os
import sys
import tempfile
import unittest
import shutil
import subprocess

# Add the parent directory to the path so we can import from lib
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

# Import the modules we're testing
try:
    from lib.settings_model import Settings
    from generate_settings import convert_settings_to_cpp
except ImportError as e:
    print(f"Error importing modules: {e}")
    print("Make sure you're running this from the Settings directory")
    sys.exit(1)


class TestGenerateSettings(unittest.TestCase):
    """Test suite for the generate_settings.py script"""

    def setUp(self):
        """Create temporary directories and files for testing"""
        self.temp_dir = tempfile.mkdtemp(prefix="ironos_test_")
        self.yaml_path = os.path.join(self.temp_dir, "test_settings.yaml")
        self.cpp_path = os.path.join(self.temp_dir, "test_settings.cpp")
        self.template_path = os.path.join(
            os.path.dirname(os.path.abspath(__file__)), "settings_gen.cpp.template"
        )

        # Create a simple test YAML file
        with open(self.yaml_path, "w") as f:
            f.write(
                """settings:
  - name: TestSetting1
    min: 0
    max: 100
    increment: 1
    default: 50
  - name: TestSetting2
    min: 10
    max: 200
    increment: 5
    default: 100
"""
            )

    def tearDown(self):
        """Clean up temporary files and directories"""
        shutil.rmtree(self.temp_dir)

    def test_load_yaml_and_convert(self):
        """Test loading YAML and converting it to C++ code using the library"""
        settings = Settings()
        settings.load_from_yaml(self.yaml_path)

        # Verify settings were loaded correctly
        self.assertEqual(len(settings.entries), 2)
        self.assertEqual(settings.entries[0].name, "TestSetting1")
        self.assertEqual(settings.entries[0].min, 0)
        self.assertEqual(settings.entries[0].max, 100)
        self.assertEqual(settings.entries[0].increment, 1)
        self.assertEqual(settings.entries[0].default, 50)

        # Convert to C++ code
        cpp_code = convert_settings_to_cpp(settings)

        # Verify the C++ code contains expected content
        self.assertIn("0", cpp_code)
        self.assertIn("100", cpp_code)
        self.assertIn("1", cpp_code)
        self.assertIn("50", cpp_code)
        self.assertIn("TestSetting1", cpp_code)
        self.assertIn("TestSetting2", cpp_code)

    def test_script_execution(self):
        """Test executing the script as a subprocess"""
        # Only run if the template file exists
        if not os.path.exists(self.template_path):
            self.skipTest("Template file not found")
            return

        # Get path to generate_settings.py
        script_path = os.path.join(
            os.path.dirname(os.path.abspath(__file__)), "generate_settings.py"
        )

        # Run the script
        result = subprocess.run(
            [sys.executable, script_path, "TestModel", self.yaml_path, self.cpp_path],
            capture_output=True,
            text=True,
        )

        # Check if the script executed successfully
        self.assertEqual(result.returncode, 0, f"Script failed with: {result.stderr}")

        # Verify the output file was created
        self.assertTrue(os.path.exists(self.cpp_path))

        # Check the content of the output file
        with open(self.cpp_path, "r") as f:
            content = f.read()
            self.assertIn("TestSetting1", content)
            self.assertIn("TestSetting2", content)


if __name__ == "__main__":
    unittest.main()
