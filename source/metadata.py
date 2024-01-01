#!/usr/bin/env python3

import json
from pathlib import Path
import os
import re
import subprocess
import sys

# Creates an index metadata json file of the hexfiles folder
# This is used by automation like the Pinecil updater

if len(sys.argv) < 2 or len(sys.argv) > 3:
    print("Usage: metadata.py OUTPUT_FILE [model]")
    print("  OUTPUT_FILE      - the name of output file in json format with meta info about binary files")
    print("  model [optional] - name of the model (as for `make model=NAME`) to scan files for explicitly (all files in source/Hexfile by default otherwise)")
    exit(1)

# If model is provided explicitly to scan related files only for json output, then process the argument
ModelName = None
if len(sys.argv) == 3:
    ModelName = sys.argv[2]
    if ModelName.endswith("_multi-lang"):
        # rename on-the-fly for direct compatibility with make targets like PINECILMODEL_multi-lang
        ModelName = ModelName.rstrip("-lang")

HERE = Path(__file__).resolve().parent

HexFileFolder = os.path.join(HERE, "Hexfile")
OutputJSONPath = os.path.join(HexFileFolder, sys.argv[1])
TranslationsFilesPath = os.path.join(HERE.parent, "Translations")

def load_json(filename: str):
    with open(filename) as f:
        return json.loads(f.read())

def read_git_tag():
    if os.environ.get("GITHUB_CI_PR_SHA", "") != "":
        return os.environ["GITHUB_CI_PR_SHA"][:7].upper()
    else:
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
output_files = [os.path.join(HexFileFolder, f) for f in sorted(os.listdir(HexFileFolder)) if os.path.isfile(os.path.join(HexFileFolder, f))]

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
        if ModelName is not None:
            # If ModelName is provided as the second argument (compatible with make model=NAME fully) but current file name doesn't match the model name, then skip it
            if not name.startswith(ModelName + "_"):
                continue
            # If build of interest is not multi-lang one but scanning one is not MODEL_LANG-ID here, then skip it to avoid mess in json between MODEL_LANG-ID & MODEL_multi'
            if not ModelName.endswith("_multi") and not re.match(r"^" + ModelName + "_" + "([A-Z]+).*$", name):
                continue
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
