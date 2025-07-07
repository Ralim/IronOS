#! python3
import yaml
from typing import List
import os
import sys
import argparse

SETTINGS_TEMPLATE_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), "settings_gen.cpp.template")


def parse_arguments()->tuple[str, str, str]:
    parser = argparse.ArgumentParser(description='Processes the settings definitions and makes a compilable C++ file.')
    parser.add_argument('model_code', help='Model code identifier (e.g., TS101, Pinecilv2)')
    parser.add_argument('settings_definitions', help='Path to the settings YAML definition file')
    parser.add_argument('output_file_path', help='Path where the generated C++ file should be written')

    print(f"Parsing command line arguments... {sys.argv[1:]}")
    if len(sys.argv) < 4:
        parser.print_help()
        sys.exit(1)
    args = parser.parse_args()

    # Check if settings definitions file exists
    if not os.path.isfile(args.settings_definitions):
        print(f"Error: Settings definition file '{args.settings_definitions}' does not exist.")
        parser.print_help()

        sys.exit(1)

    return (args.model_code, args.settings_definitions, args.output_file_path)

# Parse the command line arguments
(MODEL_CODE,SETTINGS_DEFINITIONS_PATH,SETTINGS_OUTPUT_PATH) = parse_arguments()

class SettingsEntry:
    def __init__(self, min, max, increment, default):
        self.min = min
        self.max = max
        self.increment = increment
        self.default = default


class Settings:
  settings:List[SettingsEntry] = []


def load_settings(file_path):
    with open(file_path, 'r') as f:
        data = yaml.safe_load(f)
        settings_obj = Settings()
        settings_obj.settings = data['settings']
        return settings_obj

def save_settings(settings_obj, file_path):
    with open(file_path, 'w') as f:
        yaml.dump(settings_obj.__dict__, f, indent=2)


def convert_settings_to_cpp(settings_obj):
    cpp_code = ""
    for setting in settings_obj.settings:

        cpp_code += f"    {{ {setting['min']:>22}, {setting['max']:>70}, {setting['increment']:>18}, {setting['default']:>29}}}, // {setting['name']}\r\n"
    return cpp_code


"""
 Load the settings definitions yaml file, this is used to then generate the settings_gen.cpp file.
"""

settings_data = load_settings( SETTINGS_DEFINITIONS_PATH)
cpp_code = convert_settings_to_cpp(settings_data)
with open(SETTINGS_TEMPLATE_PATH, 'r') as f:
    template_content = f.read()
# Write the generated C++ code to the output file
# Make sure the directory exists
os.makedirs(os.path.dirname(SETTINGS_OUTPUT_PATH), exist_ok=True)
with open(SETTINGS_OUTPUT_PATH, 'w') as f:
    f.write(template_content.replace("$SETTINGSTABLE", cpp_code))
