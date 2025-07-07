# IronOS Settings management

To make working with settings easier, this folder contains a easier to read definitions file for the settings in the firmware.
Utility scripts are provided to work with this file.

## Reading the existing settings from a device

This is only supported with devices that allow reading the device memory back out over USB. This **DOES NOT** work if your device shows up as a USB storage device when in programming mode.

## Writing settings in one go to a device

You can use the edit_settings.py script to generate a .bin or .hex file that can be written to the device.
If your device supports reading out the current memory, you can load your existing settings from a file you can dump from the device.

### Main Files

- `edit_settings.py` - Editing binary settings files
- `generate_settings.py` - C++ Code generation used in build

### Library Structure (`lib/` directory)

- `settings_types.py` - Common types, constants, and imports
- `settings_util.py` - Utility functions like `get_base_address` and `resolve_expression`
- `settings_model.py` - Core data models (`SettingsEntry` and `Settings` classes)
- `settings_parser.py` - Functions for parsing settings and expressions
- `settings_cli.py` - Command-line interface handling
