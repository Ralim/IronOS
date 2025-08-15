#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import re
from typing import Dict, Optional, List, Set


class ConfigParser:
    """
    Parser for IronOS configuration.h files based on the specified model.

    Extracts #define values from the appropriate BSP folder's configuration.h file,
    handling model-specific sections with #ifdef blocks.
    """

    # Mapping from model string to BSP folder name
    MODEL_TO_FOLDER = {
        "TS100": "Miniware",
        "TS80": "Miniware",
        "TS80P": "Miniware",
        "TS101": "Miniware",
        "Pinecil": "Pinecil",
        "Pinecilv2": "Pinecilv2",
        "S60": "Sequre",
        "S60P": "Sequre",
        "MHP30": "MHP30",
    }

    def __init__(self, model: str, base_path: Optional[str] = None):
        """
        Initialize the parser with the model name.

        Args:
            model: The model name (e.g., "TS100", "Pinecilv2")
            base_path: Optional path to the IronOS source root, defaults to "../Core/BSP"
                       relative to this file's location
        """
        self.model = model

        # Validate model
        if model not in self.MODEL_TO_FOLDER:
            raise ValueError(
                f"Unknown model: {model}. Supported models: {', '.join(self.MODEL_TO_FOLDER.keys())}"
            )

        self.folder = self.MODEL_TO_FOLDER[model]

        # Determine base path
        if base_path is None:
            current_dir = os.path.dirname(os.path.abspath(__file__))
            base_path = os.path.join(current_dir, "..", "Core", "BSP")
        self.base_path = base_path

        # Compute the path to the configuration file
        self.config_path = os.path.join(base_path, self.folder, "configuration.h")
        if not os.path.exists(self.config_path):
            raise FileNotFoundError(f"Configuration file not found: {self.config_path}")

    def _preprocess_content(self, content: str) -> str:
        """
        Preprocess the content by removing comments and handling line continuations.

        Args:
            content: The raw file content

        Returns:
            Preprocessed content with comments removed and line continuations handled
        """
        # Remove C-style comments
        content = re.sub(r"/\*.*?\*/", "", content, flags=re.DOTALL)
        # Remove C++-style comments
        content = re.sub(r"//.*?$", "", content, flags=re.MULTILINE)

        # Handle line continuations
        content = re.sub(r"\\\s*\n", " ", content)

        return content

    def _extract_defines(self, content: str) -> Dict[str, Optional[str]]:
        """
        Extract all #define directives from the content.

        Args:
            content: The preprocessed file content

        Returns:
            Dictionary mapping define names to their values
        """
        result = {}
        define_pattern = re.compile(
            r"#define\s+(\w+)(?:\s+(.+?))?(?:\s+//.*)?$", re.MULTILINE
        )

        for match in define_pattern.finditer(content):
            key = match.group(1)
            value = match.group(2)

            if value is not None:
                value = value.strip()
                if not value:  # Empty value after stripping
                    value = None

            result[key] = value

        return result

    def _is_valid_value(self, value: Optional[str]) -> bool:
        """
        Check if the define value is valid for inclusion.

        Args:
            value: The define value to check

        Returns:
            True if the value is numeric, False otherwise
        """
        if value is None:
            return False

        # Try to parse as an integer or float
        try:
            int(value, 0)  # Base 0 handles 0x for hex, etc.
            return True
        except ValueError:
            try:
                float(value)
                return True
            except ValueError:
                return False

    def _filter_defines(self, defines: Dict[str, Optional[str]]) -> Dict[str, int]:
        """
        Filter defines to include only those with numeric values.

        Args:
            defines: Dictionary of all defines

        Returns:
            Dictionary with only numeric defines, converted to integers
        """
        result = {}

        for key, value in defines.items():
            if self._is_valid_value(value):
                try:
                    # Try to convert to int (for hex, binary, etc.)
                    result[key] = int(value, 0)
                except ValueError:
                    try:
                        # If that fails, try float and then convert to int
                        result[key] = int(float(value))
                    except ValueError:
                        # If all conversions fail, skip this value
                        pass

        return result

    def _get_model_specific_blocks(self, content: str) -> List[tuple]:
        """
        Extract model-specific blocks from the content.

        Args:
            content: The preprocessed file content

        Returns:
            List of tuples with (model_name, block_content)
        """
        blocks = []
        model_ifdef_pattern = re.compile(
            r"#ifdef\s+MODEL_(\w+)(.*?)(?:#else.*?)?#endif", re.DOTALL
        )

        for match in model_ifdef_pattern.finditer(content):
            model_name = match.group(1)
            block_content = match.group(2)
            blocks.append((model_name, block_content))

        return blocks

    def parse(self) -> Dict[str, int]:
        """
        Parse the configuration file for the specified model.

        Returns:
            Dictionary of parsed #define values (name -> numeric value)
        """
        # Read the configuration file
        with open(self.config_path, "r", encoding="utf-8") as f:
            content = f.read()

        # Preprocess the content
        preprocessed = self._preprocess_content(content)

        # Extract all defines from the main content
        all_defines = self._extract_defines(preprocessed)

        # Get model-specific blocks
        model_blocks = self._get_model_specific_blocks(preprocessed)

        # Process model-specific blocks
        handled_keys = set()
        for block_model, block_content in model_blocks:
            # If this block is for our model or we're in a Miniware model
            if block_model == self.model or (
                self.folder == "Miniware"
                and f"MODEL_{self.model}" == f"MODEL_{block_model}"
            ):

                # Extract defines from this block
                block_defines = self._extract_defines(block_content)

                # Add to all_defines, these take precedence
                for key, value in block_defines.items():
                    all_defines[key] = value
                    handled_keys.add(key)

        # Remove keys that were in other model-specific blocks but not for our model
        for block_model, block_content in model_blocks:
            if block_model != self.model and not (
                self.folder == "Miniware"
                and f"MODEL_{self.model}" == f"MODEL_{block_model}"
            ):
                block_defines = self._extract_defines(block_content)
                for key in block_defines:
                    if key not in handled_keys and key in all_defines:
                        del all_defines[key]

        # Filter defines to only include numeric values
        numeric_defines = self._filter_defines(all_defines)

        return numeric_defines


def parse_config(model: str, base_path: Optional[str] = None) -> Dict[str, int]:
    """
    Parse the configuration for the specified model.

    Args:
        model: The model string (e.g., "TS100", "Pinecilv2")
        base_path: Optional path to the IronOS source root

    Returns:
        Dictionary of configuration values
    """
    parser = ConfigParser(model, base_path)
    return parser.parse()


if __name__ == "__main__":
    import sys
    import json

    if len(sys.argv) < 2:
        print("Usage: python config_parser.py MODEL_NAME [BASE_PATH]")
        sys.exit(1)

    model = sys.argv[1]
    base_path = sys.argv[2] if len(sys.argv) > 2 else None

    try:
        config = parse_config(model, base_path)
        print(json.dumps(config, indent=2))
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)
