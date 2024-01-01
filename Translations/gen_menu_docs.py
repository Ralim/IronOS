#!/usr/bin/env python3

import json
import logging
import os
import sys
from pathlib import Path


logging.basicConfig(stream=sys.stdout, level=logging.DEBUG)


HERE = Path(__file__).resolve().parent
TRANSLATION_DEFS_PATH = os.path.join(HERE, "translations_definitions.json")
ENGLISH_TRANSLATION_PATH = os.path.join(HERE, "translation_EN.json")
MENU_DOCS_FILE_PATH = os.path.join(HERE.parent, "Documentation/Settings.md")

# Loading a single JSON file


def load_json(filename: str, skip_first_line: bool) -> dict:
    with open(filename) as f:
        if skip_first_line:
            f.readline()
        return json.loads(f.read())


def write_header(filep):
    """
    Writes the markdown constant header area out
    """
    constant_header = """<!-- This is an automatically generated file. DO NOT EDIT. Edit gen_menu_docs.py instead -->

# IronOS Settings Menu

The below breaks down the menu's and what each setting means.
    """
    filep.write(constant_header)


def write_menu_categories(filep, defs, translation_data):
    """
    Writes the menu categories section out
    """
    menu_cat_pretense = """
## Menu Categories

In the menu there are a few main categories that are used to keep the list manageable.
"""
    filep.write(menu_cat_pretense)
    for menu in defs.get("menuGroups", {}):
        menu_id = menu.get("id", "")
        entry = translation_data.get("menuGroups", {}).get(menu_id, "")
        name = " ".join(entry.get("text2", []))
        desc = menu.get("description", "")
        section = f"""
### Category: {name}

{desc}
"""
        filep.write(section)


def write_menu_entries(filep, defs, translation_data):
    """
    Writes the menu entries section out
    """

    menu_entries_pretense = """
## Settings

These are all of the settings possible in the menu.
**Not all settings are visible for all devices.**
For example, the TS100 does not have USB-PD settings.

When using the device, if unsure you can pause (press nothing) on a setting and after a short delay help text will scroll across the screen.
This is the "on device help text".
"""
    filep.write(menu_entries_pretense)
    for menu in defs.get("menuOptions", {}):
        menu_id = menu.get("id", "")
        entry = translation_data.get("menuOptions", {}).get(menu_id, "")
        name = " ".join(entry.get("text2", []))
        desc = menu.get("description", "")
        on_device_desc = entry.get("desc", "")
        section = f"""
### Setting: {name}

{desc}

On device help text:

{on_device_desc}
"""
        filep.write(section)


def main() -> None:
    json_dir = HERE
    print(json_dir)
    logging.info("Loading translation definitions")
    defs = load_json(TRANSLATION_DEFS_PATH)
    eng_translation = load_json(ENGLISH_TRANSLATION_PATH)
    with open(MENU_DOCS_FILE_PATH, "w") as outputf:
        write_header(outputf)
        write_menu_categories(outputf, defs, eng_translation)
        write_menu_entries(outputf, defs, eng_translation)
    logging.info("Done")


if __name__ == "__main__":
    main()
