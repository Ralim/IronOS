#!/bin/bash

TRANSLATION_DIR="../../Translation Editor"
TRANSLATION_SCRIPT="make_translation.py"
TRANSLATION_FILE_PATTERN="translation_*.json"

# AVAILABLE_LANGUAGES will be calculating according to json files in $TRANSLATION_DIR
AVAILABLE_LANGUAGES=()
BUILD_LANGUAGES=()
AVAILABLE_MODELS=("TS100" "TS80")
BUILD_MODELS=()

usage ()
{
  echo "Usage : $(basename $0) [-l <LANG_CODE>] [-m <TS100|TS80>] [-h]

Parameters :
    -l LANG_CODE : Force a specific language (E.g. : EN, FR, NL_BE, ...)
    -m MODEL     : Force a specific model (E.g. : TS100 or TS80)
    -h           : Show this help message

INFO : By default, without parameters, the build is for all platforms and all languages" 1>&2
  exit 1
}

checkLastCommand ()
{
    if [ $? -eq 0 ]
    then
        echo "    [Success]"
        echo "*********************************************"
    else
        forceExit
fi
}

forceExit ()
{
    echo "    [Error]"
    echo "*********************************************"
    echo " -- Stop on error --"
    exit 1
}

isInArray ()
{
    local value="$1"   # Save first argument in a variable
    shift              # Shift all arguments to the left (original $1 gets lost)
    local array=("$@") # Rebuild the array with rest of arguments

    for item in "${array[@]}"
    do
        [[ $value == "$item" ]] && return 0
    done
    return 1
}

while getopts h:l:m: option
do
    case "${option}" in
        h)
            usage
            ;;
        l)
            LANGUAGE=${OPTARG}
            ;;
        m)
            MODEL=${OPTARG}
            ;;
        *)
            usage
            ;;
    esac
done
shift $((OPTIND-1))

echo "*********************************************"
echo "             Builder for the"
echo "      Alternate Open Source Firmware"
echo "        for Miniware TS100 or TS80"
echo "                                     by Ralim"
echo "*********************************************"

# Calculate available languages
for file in $(find "$TRANSLATION_DIR" -name "$TRANSLATION_FILE_PATTERN" -exec basename {} ';')
do
    endIdx=$((${#file}-5))
    AVAILABLE_LANGUAGES+=($(echo "$file" | cut -c13-$endIdx | tr [a-z] [A-Z]))
done

# Checking requested language
echo "Available languages :"
echo "    ${AVAILABLE_LANGUAGES[@]}"
echo "Requested languages :"
if [ -n "$LANGUAGE" ]
then
    if isInArray "$LANGUAGE" "${AVAILABLE_LANGUAGES[@]}"
    then
        echo "    $LANGUAGE" 
        BUILD_LANGUAGES+=($LANGUAGE)
    else
        echo "    $LANGUAGE doesn't exist" 
        forceExit
    fi
else
	echo "    [ALL LANGUAGES]"
    BUILD_LANGUAGES+=(${AVAILABLE_LANGUAGES[@]})
fi
echo "*********************************************"

# Checking requested model
echo "Available models :"
echo "    ${AVAILABLE_MODELS[@]}"
echo "Requested models :"
if [ -n "$MODEL" ]
then
    if isInArray "$MODEL" "${AVAILABLE_MODELS[@]}"
    then
    	echo "    $MODEL" 
        BUILD_MODELS+=($MODEL)
    else
        echo "    $MODEL doesn't exist" 
        forceExit
    fi
else
	echo "    [ALL MODELS]"
    BUILD_MODELS+=(${AVAILABLE_MODELS[@]})
fi
echo "*********************************************"

if [ ${#BUILD_LANGUAGES[@]} -gt 0 ] && [ ${#BUILD_MODELS[@]} -gt 0 ]
then 
    echo "Generating Translation.cpp"
    python "$TRANSLATION_DIR/$TRANSLATION_SCRIPT" "$TRANSLATION_DIR" 1>/dev/null
    checkLastCommand

    echo "Cleaning previous builds"
    make clean 1>/dev/null
    checkLastCommand

    for lang in "${BUILD_LANGUAGES[@]}"
    do
        for model in "${BUILD_MODELS[@]}"
        do
            echo "Building firmware for $model in $lang"
            make -j16 lang=$lang model=$model 1>/dev/null
            checkLastCommand
            rm -rf Objects/src 1>/dev/null
        done
    done
else
    echo "Nothing to build. (no model or language specified)"
    forceExit
fi
echo " -- Firmwares successfully generated --"
echo "End..."
