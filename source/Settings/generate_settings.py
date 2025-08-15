#!/usr/bin/env python3
"""
IronOS Settings Generator - Refactored

A tool to generate C++ code from settings definitions for IronOS.
This is a refactored version that uses the shared library modules.
"""

import yaml
import os
import sys
import argparse
from typing import List

# Import from the lib package
from lib.settings_model import Settings
from lib.settings_types import DEFAULT_YAML_PATH

# Constants
SETTINGS_TEMPLATE_PATH = os.path.join(
    os.path.dirname(os.path.abspath(__file__)), "settings_gen.cpp.template"
)


def parse_arguments() -> tuple[str, str, str]:
    """Parse command line arguments for the settings generator

    Returns:
        tuple: (model_code, settings_definitions_path, output_file_path)
    """
    parser = argparse.ArgumentParser(
        description="Processes the settings definitions and makes a compilable C++ file."
    )
    parser.add_argument(
        "model_code", help="Model code identifier (e.g., TS101, Pinecilv2)"
    )
    parser.add_argument(
        "settings_definitions",
        help="Path to the settings YAML definition file",
        default=DEFAULT_YAML_PATH,
        nargs="?",
    )
    parser.add_argument(
        "output_file_path", help="Path where the generated C++ file should be written"
    )

    print(f"Parsing command line arguments... {sys.argv[1:]}")
    if len(sys.argv) < 3:
        parser.print_help()
        sys.exit(1)
    args = parser.parse_args()

    # Check if settings definitions file exists
    if not os.path.isfile(args.settings_definitions):
        print(
            f"Error: Settings definition file '{args.settings_definitions}' does not exist."
        )
        parser.print_help()
        sys.exit(1)

    return (args.model_code, args.settings_definitions, args.output_file_path)


def convert_settings_to_cpp(settings) -> str:
    """Convert settings to C++ code for inclusion in a template

    Args:
        settings: Either a Settings object or a dictionary with settings data

    Returns:
        String containing formatted C++ code for settings table
    """
    cpp_code = ""

    # Handle both our Settings object format and the original dictionary format
    if hasattr(settings, "entries"):
        # New format: Settings object with entries attribute
        for entry in settings.entries:
            cpp_code += f"    {{ {entry.min:>22}, {entry.max:>70}, {entry.increment:>18}, {entry.default:>29}}}, // {entry.name}\r\n"
    else:
        # Original format: Dictionary with 'settings' key
        for setting in settings.settings:
            cpp_code += f"    {{ {setting['min']:>22}, {setting['max']:>70}, {setting['increment']:>18}, {setting['default']:>29}}}, // {setting['name']}\r\n"

    return cpp_code


def main():
    """Main function to run the settings generator"""
    # Parse command line arguments
    (model_code, settings_definitions_path, settings_output_path) = parse_arguments()

    # Initialize settings
    settings = Settings()

    # Load settings definitions from YAML
    print(f"Loading settings definitions from {settings_definitions_path}")
    try:
        settings.load_from_yaml(settings_definitions_path)
    except Exception as e:
        print(f"Error loading settings definitions: {e}")
        # Fall back to the original loading method if the new one fails
        try:
            print("Trying alternative loading method...")
            # Load using the original method from generate_settings.py
            with open(settings_definitions_path, "r") as f:
                data = yaml.safe_load(f)
                settings = type("Settings", (), {})()
                settings.settings = data["settings"]
            print("Successfully loaded settings using alternative method.")
        except Exception as nested_e:
            print(f"All loading methods failed: {nested_e}")
            sys.exit(1)

    # Convert settings to C++ code
    cpp_code = convert_settings_to_cpp(settings)

    # Load template content
    try:
        with open(SETTINGS_TEMPLATE_PATH, "r") as f:
            template_content = f.read()
    except Exception as e:
        print(f"Error reading template file: {e}")
        sys.exit(1)

    # Write the generated C++ code to the output file
    try:
        # Make sure the directory exists
        os.makedirs(os.path.dirname(settings_output_path), exist_ok=True)

        # Write the output file
        with open(settings_output_path, "w") as f:
            f.write(template_content.replace("$SETTINGSTABLE", cpp_code))

        print(f"Successfully generated C++ code at {settings_output_path}")
    except Exception as e:
        print(f"Error writing output file: {e}")
        sys.exit(1)


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\nOperation cancelled by user")
        sys.exit(1)
