#!/usr/bin/env python3
"""
IronOS Settings Editor - Refactored

A tool to edit and generate settings binary files for IronOS.
This is a refactored version of the original edit_settings.py,
with functionality split into separate modules for better maintainability.
"""

import sys
from lib import run_editing_settings_file_cli

if __name__ == "__main__":
    try:
        run_editing_settings_file_cli()
    except KeyboardInterrupt:
        print("\nOperation cancelled by user")
        sys.exit(1)
