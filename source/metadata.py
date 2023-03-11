#! python3

import json
from pathlib import Path
import os
import re
import subprocess
import sys

# Creates an index metadata json file of the hexfiles folder
# This is used by automation like the Pinecil updater


if len(sys.argv) != 2:
    print("Requires the output json name as an arg")
    exit(1)

HERE = Path(__file__).resolve().parent

HexFileFolder = os.path.join(HERE, "Hexfile")
OutputJSONPath = os.path.join(HexFileFolder, sys.argv[1])
TranslationsFilesPath = os.path.join(HERE.parent, "Translations")


def load_json(filename: str):
    with open(filename) as f:
        return json.loads(f.read())


def read_git_tag():
    return f"{subprocess.check_output(['git', 'rev-parse', '--short=7', 'HEAD']).strip().decode('ascii').upper()}"


def read_version():
    with open(HERE / "version.h") as version_file:
        for line in version_file:
            if re.findall(r"^.*(?<=(#define)).*(?<=(BUILD_VERSION))", line):
                matches = re.findall(r"\"(.+?)\"", line)
                if matches:
                    return matches[0]
    raise Exception("Could not parse version")


# Fetch our file listings
translation_files = [os.path.join(TranslationsFilesPath, f) for f in os.listdir(TranslationsFilesPath) if os.path.isfile(os.path.join(TranslationsFilesPath, f)) and f.endswith(".json")]
output_files = [os.path.join(HexFileFolder, f) for f in os.listdir(HexFileFolder) if os.path.isfile(os.path.join(HexFileFolder, f))]

parsed_languages = {}
for path in translation_files:
    lang: dict = load_json(path)
    code = lang.get("languageCode", None)
    if code is not None:
        parsed_languages[code] = lang

# Now that we have the languages, we can generate our index of info on each file

output_json = {"git_tag": read_git_tag(), "release": read_version(), "contents": {}}

device_model_name = None
for file_path in output_files:
    if file_path.endswith(".hex") or file_path.endswith(".dfu"):
        # Find out what language this file is
        name: str = os.path.basename(file_path)
        matches = re.findall(r"^([a-zA-Z0-9]+)_(.+)\.(.+)$", name)
        if matches:
            matches = matches[0]
            if len(matches) == 3:
                if device_model_name is None:
                    device_model_name = matches[0]
                lang_code: str = matches[1]
                lang_file = parsed_languages.get(lang_code, None)
                if lang_file is None and lang_code.startswith("multi_"):
                    # Multi files wont match, but we fake this by just taking the filename to it
                    lang_file = {"languageLocalName": lang_code.replace("multi_", "").replace("compressed_", "")}
                if lang_file is None:
                    raise Exception(f"Could not match language code {lang_code}")
                file_record = {"language_code": lang_code, "language_name": lang_file.get("languageLocalName", None)}
                output_json["contents"][name] = file_record
            else:
                print(f"failed to parse {matches}")

if device_model_name is None:
    raise Exception("No files parsed")

output_json["model"] = device_model_name
with open(OutputJSONPath, "w", encoding="utf8") as json_file:
    json.dump(output_json, json_file, ensure_ascii=False)
