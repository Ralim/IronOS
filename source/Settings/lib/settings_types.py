#!/usr/bin/env python3
"""
settings_types.py - Common types and constants for IronOS settings management
"""

import os
from typing import Dict, Any

# Try to import IntelHex, which is optional
try:
    from intelhex import IntelHex

    HEX_SUPPORT = True
except ImportError:
    IntelHex = None
    HEX_SUPPORT = False

# Constants
DEFAULT_YAML_PATH = os.path.join(
    os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "settings.yaml"
)

# Type aliases
SettingsDict = Dict[str, Any]
