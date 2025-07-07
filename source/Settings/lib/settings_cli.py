#!/usr/bin/env python3
"""
settings_cli.py - Command line interface for IronOS settings management
"""

import sys
import os
import argparse
from typing import Tuple

# Import local modules
from .settings_types import DEFAULT_YAML_PATH, HEX_SUPPORT
from .settings_model import Settings
from .settings_util import get_base_address
from .settings_parser import process_default_values

# Import the config_parser module from parent directory
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
try:
    from config_parser import parse_config
except ImportError:

    def parse_config(model):
        print(
            f"Warning: config_parser module not found, BSP configuration for model {model} not available"
        )
        return {}


def parse_arguments():
    """Parse command line arguments"""
    parser = argparse.ArgumentParser(description="Edit IronOS settings")
    parser.add_argument(
        "-i", "--input", help="Input binary or hex settings file (optional)"
    )
    parser.add_argument(
        "-o",
        "--output",
        help="Output binary settings file or hex file (use .hex extension for Intel HEX format)",
        required=True,
    )
    parser.add_argument(
        "-d",
        "--definitions",
        help="Settings definitions YAML file",
        default=DEFAULT_YAML_PATH,
    )
    parser.add_argument(
        "-m",
        "--model",
        help="Device model ID (required for Intel HEX output to set correct base address)",
        choices=[
            "TS100",
            "TS80",
            "TS80P",
            "TS101",
            "S60",
            "S60P",
            "Pinecil",
            "Pinecilv2",
            "MHP30",
        ],
    )
    parser.add_argument(
        "-n",
        "--non-interactive",
        help="Non-interactive mode (uses default values)",
        action="store_true",
    )
    parser.add_argument(
        "--use-bsp-defaults",
        help="Use values from BSP configuration.h for non-numeric settings",
        action="store_true",
    )
    parser.add_argument("--debug", help="Enable debug output", action="store_true")

    return parser.parse_args()


def handle_input_file(args, settings) -> Tuple[Settings, int]:
    """Load settings from input file if provided and return base address

    Args:
        args: Command line arguments
        settings: Settings object

    Returns:
        The detected base address from the input file (0 if not available)
    """
    input_base_address = 0
    bsp_config = None

    # If model is provided, load the BSP configuration
    if args.model:
        try:
            print(f"Loading BSP configuration for model {args.model}")
            bsp_config = parse_config(args.model)
            print(f"Loaded {len(bsp_config)} configuration values from BSP")

            # Add common default values that might be missing from the BSP config
            if "QC_VOLTAGE_MAX" not in bsp_config:
                bsp_config["QC_VOLTAGE_MAX"] = 120

        except Exception as e:
            print(f"Error loading BSP configuration: {e}")
            print("Will use YAML defaults instead")
            bsp_config = None

    # If input file is provided, load settings from it
    if args.input:
        file_type = "hex" if args.input.lower().endswith(".hex") else "binary"
        print(f"Loading settings from {file_type} file {args.input}")
        success, input_base_address = settings.load_from_binary(args.input)

        if not success:
            print("Using default values from settings definitions")
            process_default_values(
                settings, bsp_config, args.debug if hasattr(args, "debug") else False
            )
    else:
        print("No input file provided, using default values from settings definitions")
        process_default_values(
            settings, bsp_config, args.debug if hasattr(args, "debug") else False
        )

    return (settings, input_base_address)


def run_editing_settings_file_cli():
    """Main function to run the CLI"""
    args = parse_arguments()

    # Check if settings definitions file exists
    if not os.path.isfile(args.definitions):
        print(f"Error: Settings definition file '{args.definitions}' does not exist.")
        sys.exit(1)

    # Initialize settings
    settings = Settings()

    # Load settings definitions from YAML
    print(f"Loading settings definitions from {args.definitions}")
    try:
        settings.load_from_yaml(args.definitions)
    except Exception as e:
        print(f"Error loading settings definitions: {e}")
        sys.exit(1)

    # Initialize base_address
    base_address = 0
    input_base_address = 0

    # Handle input file and process defaults
    (settings, input_base_address) = handle_input_file(args, settings)

    # Determine the base address to use for output
    # Priority: 1. Model-specified base address, 2. Input hex file base address, 3. Default (0)
    if args.model:
        base_address = get_base_address(args.model)
        print(f"Using base address 0x{base_address:08X} for model {args.model}")
    elif input_base_address > 0:
        base_address = input_base_address
        print(f"Using base address 0x{base_address:08X} from input file")

    # If we have a model, try to get SETTINGS_START_PAGE from BSP config
    if args.model and not args.input and base_address == 0:
        try:
            bsp_config = parse_config(args.model)
            if "SETTINGS_START_PAGE" in bsp_config:
                # Use the settings page address from BSP if available
                base_address = bsp_config["SETTINGS_START_PAGE"]
                print(f"Using SETTINGS_START_PAGE from BSP: 0x{base_address:08X}")
        except Exception as e:
            print(f"Failed to get flash address from BSP: {e}")

    # Edit settings if not in non-interactive mode
    if not args.non_interactive:
        settings.edit_all_settings()
    else:
        print("Running in non-interactive mode, using loaded/default values")

    # Check if output is hex and we need intelhex module
    if args.output.lower().endswith(".hex"):
        if not HEX_SUPPORT:
            print(
                "Error: Output file has .hex extension but intelhex module is not installed."
            )
            print("Install it with 'pip install intelhex' to generate Intel HEX files.")
            print(
                "Please change the output file extension to .bin or install the IntelHex module."
            )
            sys.exit(1)
        elif not args.model and input_base_address == 0:
            print("Warning: No base address available for HEX output.")
            print(
                "Please specify a model with the --model option or use an input hex file with a valid base address."
            )
            sys.exit(1)

    # Save settings to binary or hex file
    print(f"\nSaving settings to {args.output}")
    if not settings.save_to_binary(args.output, base_address):
        print("Failed to save settings")
        sys.exit(1)

    print("Settings saved successfully")
