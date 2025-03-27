#! python3
import yaml
from typing import List
import os

SETTINGS_OUTPUT_PATH = os.path.join(os.path.dirname(__file__), '../Core/Gen/settings_gen.cpp')

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

settings_data = load_settings( 'settings.yaml')
cpp_code = convert_settings_to_cpp(settings_data)
with open("settings_gen.cpp.template", 'r') as f:
    template_content = f.read()
with open(SETTINGS_OUTPUT_PATH, 'w') as f:
    f.write(template_content.replace("$SETTINGSTABLE", cpp_code))
