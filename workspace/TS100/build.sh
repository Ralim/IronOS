#!/bin/bash

TRANSLATION_DIR="../../Translation Editor"
TRANSLATION_SCRIPT="make_translation.py"

# AVAILABLE_LANGUAGES will be calculating according to json files in $TRANSLATION_DIR
AVAILABLE_LANGUAGES=()
BUILD_LANGUAGES=()
AVAILABLE_MODELS=("TS100" "TS80")
BUILD_MODELS=()

usage() {
    echo "Usage : $(basename "$0") [-l <LANG_CODE>] [-m <TS100|TS80>] [-h]

Parameters :
    -l LANG_CODE : Force a specific language (E.g. : EN, FR, NL_BE, ...)
    -m MODEL     : Force a specific model (E.g. : TS100 or TS80)
    -h           : Show this help message

INFO : By default, without parameters, the build is for all platforms and all languages" 1>&2
    exit 1
}

checkLastCommand() {
    if [ $? -eq 0 ]; then
        echo "    [Success]"
        echo "*********************************************"
    else
        forceExit
    fi
}

forceExit() {
    echo "    [Error]"
    echo "*********************************************"
    echo " -- Stop on error --"
    exit 1
}

isInArray() {
    local value="$1"   # Save first argument in a variable
    shift              # Shift all arguments to the left (original $1 gets lost)
    local array=("$@") # Rebuild the array with rest of arguments

    for item in "${array[@]}"; do
        [[ $value == "$item" ]] && return 0
    done
    return 1
}

while getopts h:l:m: option; do
    case "${option}" in
    h)
        usage
        ;;
    l)
        LANGUAGEREQ=${OPTARG}
        ;;
    m)
        MODEL=${OPTARG}
        ;;
    *)
        usage
        ;;
    esac
done
shift $((OPTIND - 1))

echo "*********************************************"
echo "             Builder for the"
echo "      Alternate Open Source Firmware"
echo "        for Miniware TS100 or TS80"
echo "                                     by Ralim"
echo "                                             "
echo "*********************************************"

# Calculate available languages
for f in "$TRANSLATION_DIR"/translation_*.json; do
    AVAILABLE_LANGUAGES+=($(echo $f | tr "[:lower:]" "[:upper:]" | sed "s/[^_]*_//" | sed "s/\.JSON//g"))
done

# Checking requested language
echo "Available languages :"
echo "    ${AVAILABLE_LANGUAGES[*]}"
echo "Requested languages :"
if [ -n "$LANGUAGEREQ" ]; then
    if isInArray "$LANGUAGEREQ" "${AVAILABLE_LANGUAGES[@]}"; then
        echo "    $LANGUAGEREQ"
        BUILD_LANGUAGES+=("$LANGUAGEREQ")
    else
        echo "    $LANGUAGEREQ doesn't exist"
        forceExit
    fi
else
    echo "    [ALL LANGUAGES]"
    BUILD_LANGUAGES+=("${AVAILABLE_LANGUAGES[@]}")
fi
echo "*********************************************"

# Checking requested model
echo "Available models :"
echo "    ${AVAILABLE_MODELS[*]}"
echo "Requested models :"
if [ -n "$MODEL" ]; then
    if isInArray "$MODEL" "${AVAILABLE_MODELS[@]}"; then
        echo "    $MODEL"
        BUILD_MODELS+=("$MODEL")
    else
        echo "    $MODEL doesn't exist"
        forceExit
    fi
else
    echo "    [ALL MODELS]"
    BUILD_MODELS+=("${AVAILABLE_MODELS[@]}")
fi
echo "*********************************************"

if [ ${#BUILD_LANGUAGES[@]} -gt 0 ] && [ ${#BUILD_MODELS[@]} -gt 0 ]; then
    echo "Generating Translation.cpp"
    python3 "$TRANSLATION_DIR/$TRANSLATION_SCRIPT" "$TRANSLATION_DIR"
    checkLastCommand

    echo "Cleaning previous builds"
    rm -rf Hexfile/ >/dev/null
    rm -rf Objects/ >/dev/null
    make clean >/dev/null
    checkLastCommand

    for model in "${BUILD_MODELS[@]}"; do
        for lang in "${BUILD_LANGUAGES[@]}"; do
            echo "Building firmware for $model in $lang"
            make -j lang="$lang" model="$model" >/dev/null
            checkLastCommand
            echo "Cleanup Temp files for $model in $lang"
            rm -rf Objects/Core/ >/dev/null
            checkLastCommand
        done
        echo "Cleanup model change"
        rm -rf Objects/ >/dev/null
        checkLastCommand
    done
else
    echo "Nothing to build. (no model or language specified)"
    forceExit
fi
echo " -- Firmwares successfully generated --"
echo "End..."
