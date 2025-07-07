#!/usr/bin/env python3
"""
settings_model.py - Settings data models for IronOS
"""

import sys
import yaml
import struct
import os
import re
from typing import List, Dict, Optional, Tuple, Any, Union

# Import local modules
from .settings_types import HEX_SUPPORT, IntelHex
from .settings_util import resolve_expression


class SettingsEntry:
    """Represents a single settings entry definition"""

    def __init__(self, min_value, max_value, increment, default, name):
        self.min = min_value
        self.max = max_value
        self.increment = increment
        self.default = default
        self.name = name

    def __str__(self):
        return f"{self.name}: {self.default} (min: {self.min}, max: {self.max}, increment: {self.increment})"


class Settings:
    """Manages a collection of settings entries and their values"""

    def __init__(self):
        self.entries: List[SettingsEntry] = []
        self.values: List[int] = []

    def load_from_yaml(self, file_path: str) -> None:
        """Load settings definitions from YAML file"""
        with open(file_path, "r") as f:
            data = yaml.safe_load(f)

        self.entries = []
        self.values = []

        for setting in data["settings"]:
            # Some values in the YAML might use expressions referencing other values
            # We'll keep them as strings for now and resolve them later
            entry = SettingsEntry(
                setting["min"],
                setting["max"],
                setting["increment"],
                setting["default"],
                setting["name"],
            )
            self.entries.append(entry)

            # Try to convert default value to int if possible
            default_value = setting["default"]
            if isinstance(default_value, int):
                self.values.append(default_value)
            else:
                try:
                    self.values.append(int(default_value))
                except (ValueError, TypeError):
                    self.values.append(default_value)

    def load_from_binary(self, file_path: str) -> Tuple[bool, int]:
        """Load settings from a binary or hex file

        Args:
            file_path: Path to the binary or hex file

        Returns:
            Tuple of (success, base_address)
            success: True if settings were loaded successfully
            base_address: The base address of the settings in the flash memory (0 if not applicable)
        """
        # Check file extension to determine format
        is_hex_file = file_path.lower().endswith(".hex")

        if is_hex_file and not HEX_SUPPORT:
            print(
                "Error: Cannot load .hex file because intelhex module is not installed."
            )
            print(
                "Install it with 'pip install intelhex' to work with Intel HEX files."
            )
            return False, 0

        # Read the file
        try:
            if is_hex_file:
                ih = IntelHex(file_path)
                # Find the address range of data in the hex file
                start_addr = ih.minaddr()
                end_addr = ih.maxaddr()

                if end_addr - start_addr < 64:
                    print(
                        f"Warning: Hex file contains very little data ({end_addr - start_addr + 1} bytes)"
                    )

                # Extract the binary data from the hex file
                binary_data = ih.tobinstr(
                    start=start_addr, size=end_addr - start_addr + 1
                )
                base_address = start_addr
            else:
                with open(file_path, "rb") as f:
                    binary_data = f.read()
                base_address = 0

            # Check if file size is correct
            expected_size = len(self.entries) * 2  # 2 bytes per setting
            if len(binary_data) < expected_size:
                print(
                    f"Warning: File size ({len(binary_data)} bytes) is smaller than expected ({expected_size} bytes)"
                )
                print(
                    "File may be truncated or corrupted. Will read as many settings as possible."
                )

            # Parse settings values
            for i in range(min(len(self.entries), len(binary_data) // 2)):
                # Read 16-bit value (little-endian)
                value = struct.unpack("<H", binary_data[i * 2 : i * 2 + 2])[0]

                # Apply bounds checking
                min_val = self.entries[i].min
                max_val = self.entries[i].max

                # If min/max are strings, try to convert to integers
                if isinstance(min_val, str):
                    try:
                        min_val = int(min_val)
                    except ValueError:
                        min_val = 0

                if isinstance(max_val, str):
                    try:
                        max_val = int(max_val)
                    except ValueError:
                        max_val = 65535

                if value < min_val:
                    print(
                        f"Warning: Setting {self.entries[i].name} value {value} is below minimum {min_val}, clamping"
                    )
                    value = min_val
                elif value > max_val:
                    print(
                        f"Warning: Setting {self.entries[i].name} value {value} is above maximum {max_val}, clamping"
                    )
                    value = max_val

                self.values[i] = value

            print(
                f"Successfully loaded {min(len(self.entries), len(binary_data) // 2)} settings from {file_path}"
            )
            return True, base_address

        except Exception as e:
            print(f"Error loading settings from file: {e}")
            return False, 0

    def save_to_binary(self, file_path: str, base_address: int = 0) -> bool:
        """Save settings to a binary or hex file

        Args:
            file_path: Path to the output file
            base_address: Base address for the settings in flash memory (used only for hex files)

        Returns:
            True if settings were saved successfully
        """
        # Make sure all values are resolved to integers
        for i in range(len(self.values)):
            if not isinstance(self.values[i], int):
                print(
                    f"Error: Setting {self.entries[i].name} value '{self.values[i]}' is not an integer"
                )
                return False

        # Create binary data
        binary_data = bytearray()
        for value in self.values:
            # Pack as 16-bit little-endian
            binary_data.extend(struct.pack("<H", value))

        # Check file extension to determine format
        is_hex_file = file_path.lower().endswith(".hex")

        if is_hex_file and not HEX_SUPPORT:
            print(
                "Error: Cannot save as .hex file because intelhex module is not installed."
            )
            print("Install it with 'pip install intelhex' to create Intel HEX files.")
            return False

        try:
            if is_hex_file:
                # Create a new Intel HEX object
                ih = IntelHex()

                # Add the binary data at the specified base address
                for i, byte_val in enumerate(binary_data):
                    ih[base_address + i] = byte_val

                # Save to file
                ih.write_hex_file(file_path)
            else:
                # Save directly as binary
                with open(file_path, "wb") as f:
                    f.write(binary_data)

            return True
        except Exception as e:
            print(f"Error saving settings to file: {e}")
            return False

    def edit_all_settings(self) -> None:
        """Interactive editor for all settings"""
        print("\nEditing settings (press Enter to keep current value):")
        for i, entry in enumerate(self.entries):
            value = self.values[i]

            # Format current value, min and max for display
            if isinstance(value, int):
                current = str(value)
            else:
                current = f"'{value}' (unresolved)"

            # Get the raw min/max/increment values for display
            min_val = entry.min
            max_val = entry.max
            increment = entry.increment

            # Format prompt with range and increment (if not 1)
            range_text = f"[{min_val}-{max_val}]"
            if increment != 1:
                range_text = f"{range_text} step {increment}"
            prompt = f"{i+1}. {entry.name} ({current}) {range_text}: "

            # Get user input
            while True:
                user_input = input(prompt)

                # Empty input = keep current value
                if not user_input:
                    break

                # Try to parse input as integer
                try:
                    new_value = int(user_input)

                    # Check if value is in range
                    # Convert min/max to integers for validation
                    min_int = min_val
                    max_int = max_val
                    inc_int = increment

                    if isinstance(min_int, str):
                        try:
                            min_int = int(min_int)
                        except ValueError:
                            min_int = 0

                    if isinstance(max_int, str):
                        try:
                            max_int = int(max_int)
                        except ValueError:
                            max_int = 65535

                    if isinstance(inc_int, str):
                        try:
                            inc_int = int(inc_int)
                        except ValueError:
                            inc_int = 1

                    if new_value < min_int or new_value > max_int:
                        print(f"Value must be between {min_int} and {max_int}")
                        continue

                    # Check if value respects the increment step
                    if inc_int > 1:
                        # Check if value is min_int + n*inc_int
                        if (new_value - min_int) % inc_int != 0:
                            print(f"Value must be {min_int} + n*{inc_int}")
                            continue

                    # Value is valid, update it
                    self.values[i] = new_value
                    break
                except ValueError:
                    print("Invalid input, please enter a number")
