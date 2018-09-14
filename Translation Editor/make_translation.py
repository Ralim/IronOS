import json
import os
import io
import sys

TRANSLATION_CPP = "Translation.cpp"

try :
    to_unicode = unicode
except NameError:
    to_unicode = str


# Loading a single JSON file
def loadJson(fileName, skipFirstLine):
    with open(fileName) as f:
        if skipFirstLine:
            f.readline()

        obj = json.loads(f.read())

    return obj


# Reading all language translations into a dictionary by langCode
def readTranslations(jsonDir):
    langDict = {}
    
    # Read all translation files from the input dir
    for fileName in os.listdir(jsonDir):
        
        fileWithPath = os.path.join(jsonDir, fileName)
        lf = fileName.lower()
    
        # Read only translation_XX.json
        if lf.startswith("translation_") and lf.endswith(".json"):
            lang = loadJson(fileWithPath, False)
            
            # Extract lang code from file name
            langCode = fileName[12:-5].upper()
            # ...and the one specified in the JSON file...
            try:
                langCodeFromJson = lang['languageCode']
            except KeyError:
                langCodeFromJson = "(missing)"
                
            # ...cause they should be the same!
            if langCode != langCodeFromJson:
                raise ValueError("Invalid languageCode " + langCodeFromJson + " in file " + fileName)
            
            langDict[langCode] = lang
    
    return langDict


def writeStart(f):
    f.write(to_unicode("""#include "Translation.h"
#ifndef LANG
#define LANG_EN
#endif
"""))


def escapeC(s):
    return s.replace("\"", "\\\"");


def writeLanguage(languageCode):
    print "Generating block for " + languageCode 
    lang = langDict[languageCode]
    
    f.write(to_unicode("\n#ifdef LANG_" + languageCode + "\n"))
    try:
        langName = lang['languageLocalName']
    except KeyError:
        langName = languageCode
         
    f.write(to_unicode("// ---- " + langName + " ----\n\n"))

    # ----- Writing SettingsDescriptions
    obj = lang['menuOptions']
    f.write(to_unicode("const char* SettingsDescriptions[" + str(len(obj)) + "] = {\n"))
    
    maxLen = 25
    for mod in defs['menuOptions']:
        eid = mod['id']
        f.write(to_unicode("  /* " + eid.ljust(maxLen)[:maxLen] + " */ "))
        f.write(to_unicode("\"" + escapeC(obj[eid]['desc']) + "\",\n"))

    f.write(to_unicode("};\n\n"))
    
    # ----- Writing Message strings
    
    obj = lang['messages']
    
    for mod in defs['messages']:
        eid = mod['id']
        f.write(to_unicode("const char* " + eid + " = \"" + escapeC(obj[eid]) + "\";\n"))

    f.write(to_unicode("\n"))

    # ----- Writing Characters
    
    obj = lang['characters']
    
    for mod in defs['characters']:
        eid = mod['id']
        f.write(to_unicode("const char " + eid + " = '" + obj[eid] + "';\n"))

    f.write(to_unicode("\n"))
    
    # ----- Menu Options

    # Menu type
    f.write(to_unicode("const enum ShortNameType SettingsShortNameType = SHORT_NAME_" + ("DOUBLE" if lang['menuDouble'] else "SINGLE") + "_LINE;\n"))

    # ----- Writing SettingsDescriptions
    obj = lang['menuOptions']
    f.write(to_unicode("const char* SettingsShortNames[" + str(len(obj)) + "][2] = {\n"))
    
    maxLen = 25
    for mod in defs['menuOptions']:
        eid = mod['id']
        f.write(to_unicode("  /* " + eid.ljust(maxLen)[:maxLen] + " */ "))
        if lang['menuDouble']:
            f.write(to_unicode("{ \"" + escapeC(obj[eid]['text2'][0]) + "\", \"" + escapeC(obj[eid]['text2'][1]) + "\" },\n"))
        else:
            f.write(to_unicode("\"" + escapeC(obj[eid]['text']) + "\",\n"))
                
    f.write(to_unicode("};\n\n"))

    # ----- Writing Menu Groups
    obj = lang['menuGroups']
    f.write(to_unicode("const char* SettingsMenuEntries[" + str(len(obj)) + "] = {\n"))
    
    maxLen = 25
    for mod in defs['menuGroups']:
        eid = mod['id']
        f.write(to_unicode("  /* " + eid.ljust(maxLen)[:maxLen] + " */ "))
        f.write(to_unicode("\"" + escapeC(obj[eid]['text2'][0] + "\\n" + obj[eid]['text2'][1]) + "\",\n"))
                
    f.write(to_unicode("};\n\n"))

    # ----- Writing Menu Groups Descriptions
    obj = lang['menuGroups']
    f.write(to_unicode("const char* SettingsMenuEntriesDescriptions[" + str(len(obj)) + "] = {\n"))
    
    maxLen = 25
    for mod in defs['menuGroups']:
        eid = mod['id']
        f.write(to_unicode("  /* " + eid.ljust(maxLen)[:maxLen] + " */ "))
        f.write(to_unicode("\"" + escapeC(obj[eid]['desc']) + "\",\n"))
                
    f.write(to_unicode("};\n\n"))

    # ----- Block end    
    f.write(to_unicode("#endif\n"))

    
''' Reading input parameters
First parameter = json directory
Second parameter = target directory
'''


print "Making " + TRANSLATION_CPP + ":"

if len(sys.argv) > 1:
    jsonDir = sys.argv[1]
else:
    jsonDir = "."

jsonDir = os.path.abspath(jsonDir)

if len(sys.argv) > 2:
    outDir = sys.argv[2]
else:
    outDir = jsonDir

langDict = readTranslations(jsonDir)

# Read definition (required for ordering)

defs = loadJson(os.path.join(jsonDir, "translations_def.js"), True)

# These languages go first
mandatoryOrder = ['EN']

# Then add all others in alphabetical order
sortedKeys = langDict.keys()
sortedKeys.sort()

for key in sortedKeys:
    if key not in mandatoryOrder:
        mandatoryOrder.append(key)
    
# Add the rest as they come

# Start writing the file
targetTranslationFile = os.path.join(outDir, TRANSLATION_CPP)

with io.open(targetTranslationFile, 'w', encoding='utf-8', newline="\n") as f:
    writeStart(f)
    
    for langCode in mandatoryOrder:
        writeLanguage(langCode)
    
    print "Done"
