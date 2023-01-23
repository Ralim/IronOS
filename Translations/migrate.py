#!/usr/bin/env python3

import json
import os
import sys

# Migrate json files to use "\n" encoding rather than []


def load_json(filename: str) -> dict:
    with open(filename, "r", encoding="utf8") as f:
        return json.loads(f.read())


def save_json(filename: str, data: dict):
    with open(filename, "w", encoding="utf8") as f:
        json.dump(data, f, indent=4, ensure_ascii=False)


file_name = sys.argv[1]
print(file_name)

data = load_json(file_name)

# Migrate messages to be delimited
for key in data["messagesWarn"]:
    old_message = data["messagesWarn"][key]
    if isinstance(old_message, list):
        print(old_message)
        new_message = "\n".join(old_message)
        data["messagesWarn"][key] = {"message": new_message}
    else:
        data["messagesWarn"][key] = {"message": old_message}

for key in data["messages"]:
    old_message = data["messages"][key]
    if isinstance(old_message, list):
        print(old_message)
        new_message = "\n".join(old_message)
        data["messagesWarn"][key] = {"message": new_message}
    else:
        data["messagesWarn"][key] = {"message": old_message}

del data["messages"]
print("Part 2")
# for menu-groups break out the text2 field
for key in data["menuGroups"]:
    old_data = data["menuGroups"][key]
    if isinstance(old_data.get("text2", ""), list):
        new_data = "\n".join(old_data["text2"])
        data["menuGroups"][key]["displayText"] = new_data
        del data["menuGroups"][key]["text2"]
    else:
        data["menuGroups"][key]["displayText"] = old_data["text2"].replace("\n", "")
        del data["menuGroups"][key]["text2"]
    data["menuGroups"][key]["description"] = data["menuGroups"][key]["desc"]
    del data["menuGroups"][key]["desc"]


print("Part 3")
# for menu-groups break out the text2 field
for key in data["menuOptions"]:
    old_data = data["menuOptions"][key]
    if isinstance(old_data.get("text2", ""), list):
        new_data = "\n".join(old_data["text2"])
        data["menuOptions"][key]["displayText"] = new_data
        del data["menuOptions"][key]["text2"]
    else:
        data["menuOptions"][key]["displayText"] = old_data["text2"].replace("\n", "")
        del data["menuOptions"][key]["text2"]
    data["menuOptions"][key]["description"] = data["menuOptions"][key]["desc"]
    del data["menuOptions"][key]["desc"]


save_json(file_name, data)
