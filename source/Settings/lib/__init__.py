#!/usr/bin/env python3
"""
IronOS Settings Management Package

This package contains modules for managing IronOS settings:
- settings_types: Common types and constants
- settings_util: Utility functions
- settings_model: Data models for settings
- settings_parser: Functions for parsing settings
- settings_cli: Command-line interface
"""

from .settings_types import DEFAULT_YAML_PATH, HEX_SUPPORT
from .settings_util import get_base_address, resolve_expression
from .settings_model import SettingsEntry, Settings
from .settings_parser import process_default_values
from .settings_cli import (
    parse_arguments,
    handle_input_file,
    run_editing_settings_file_cli,
)

__all__ = [
    "DEFAULT_YAML_PATH",
    "HEX_SUPPORT",
    "get_base_address",
    "resolve_expression",
    "SettingsEntry",
    "Settings",
    "process_default_values",
    "parse_arguments",
    "handle_input_file",
    "run_editing_settings_file_cli",
]
