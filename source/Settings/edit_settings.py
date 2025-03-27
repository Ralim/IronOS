#! python3
import yaml
from typing import List
import os


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


settings_data = load_settings( 'settings.yaml')

"""
If the user has provided a settings binary dump, we load it and update the settings data
If the user has requested to edit the settings, we provide a menu to edit each setting
"""
