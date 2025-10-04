#!/usr/bin/env bash
set -e

TRANSLATION_DIR="../Translations"
AVAILABLE_LANGUAGES=()
BUILD_LANGUAGES=()
AVAILABLE_MODELS=("TS100" "TS80" "TS80P" "Pinecil" "MHP30" "Pinecilv2" "S60" "S60P" "T55" "TS101")
BUILD_MODELS=()
OPTIONS=()
BUILD_TYPE="Release"
CLEAN=false
VERBOSE=false
JOBS=$(nproc)

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

builder_info() {
    echo -e "${BLUE}
********************************************
               IronOS Firmware
        CMake builder for Miniware + Pine64

                                    by Ralim
********************************************${NC}"
}

# Calculate available languages
for f in "$TRANSLATION_DIR"/translation_*.json; do
    if [ -f "$f" ]; then
        AVAILABLE_LANGUAGES+=("$(echo "$f" | tr "[:lower:]" "[:upper:]" | sed "s/[^_]*_//" | sed "s/\.JSON//g")")
    fi
done

usage() {
    builder_info
    echo -e "
Usage:
    $(basename "$0") [-l <LANG_CODES>] [-m <MODELS>] [-o <OPTIONS>] [-t <BUILD_TYPE>] [-c] [-v] [-j <JOBS>] [-h]

Parameters:
    -l LANG_CODE  : Force a specific language (${AVAILABLE_LANGUAGES[*]})
    -m MODEL      : Force a specific model (${AVAILABLE_MODELS[*]})
    -o key=val    : Pass options to CMake (e.g., ws2812b_enable=1, swd_enable=1)
    -t BUILD_TYPE : Build type: Debug, Release, RelWithDebInfo, MinSizeRel (default: Release)
    -c            : Clean build directories before building
    -v            : Verbose build output
    -j JOBS       : Number of parallel jobs (default: $(nproc))
    -h            : Show this help message

Examples:
    $(basename "$0") -l EN -m TS100                           (Build one language and model)
    $(basename "$0") -l EN -m \"TS100 MHP30\"                   (Build one language and multi models)
    $(basename "$0") -l \"DE EN\" -m \"TS100 MHP30\"              (Build multi languages and models)
    $(basename "$0") -l EN -m Pinecilv2 -o ws2812b_enable=1    (Build with WS2812B support)
    $(basename "$0") -c -v                                    (Clean build and verbose output)

INFO:
    By default, without parameters, the build is for all platforms and all languages.
    All build files are placed in Objects/<model>/ subdirectories.
    Generated firmware files are placed in build-<model>/Hexfile/.

" 1>&2
    exit 1
}

checkLastCommand() {
    if [ $? -eq 0 ]; then
        echo -e "    ${GREEN}[Success]${NC}"
        echo -e "${BLUE}********************************************${NC}"
    else
        forceExit "Build failed"
    fi
}

forceExit() {
    if [ -n "$*" ]; then
        echo -e "\n\n    ${RED}[Error]: $*${NC}"
    else
        echo -e "    ${RED}[Error]${NC}"
    fi
    echo -e "${BLUE}********************************************${NC}"
    echo -e "${RED} -- Stop on error --${NC}"
    exit 1
}

isInArray() {
    local value="$1"
    shift
    local array=("$@")

    for item in "${array[@]}"; do
        [[ $value == "$item" ]] && return 0
    done
    return 1
}

# Clean all build artifacts
clean_all() {
    echo -e "${YELLOW}Cleaning all build directories and generated files...${NC}"

    # Remove Objects directory which contains all build artifacts
    if [[ -d "Objects" ]]; then
        echo "  ✓ Removed Objects/"
        rm -rf "Objects"
    fi

    # Remove generated files
    if [[ -d "Core/Gen" ]]; then
        echo "  ✓ Removed Core/Gen/"
        rm -rf "Core/Gen"
    fi

    if [[ -d "Hexfile" ]]; then
        echo "  ✓ Removed Hexfile/"
        rm -rf "Hexfile"
    fi

    echo -e "${GREEN}✓ All build artifacts cleaned!${NC}"
}

# Build firmware for a specific model and language
build_firmware() {
    local model="$1"
    local language="$2"
    local build_dir="Objects/${model}/build"

    echo -e "${YELLOW}Building ${model} firmware with ${language} language...${NC}"

    # Create build directory in Objects folder
    mkdir -p "${build_dir}"
    cd "${build_dir}"

    # Prepare CMake arguments - point to source directory
    local cmake_args=(
        "-DMODEL=${model}"
        "-DCMAKE_BUILD_TYPE=${BUILD_TYPE}"
    )

    # Add optional arguments from -o options
    for option in "${OPTIONS[@]}"; do
        if [[ "$option" == *"="* ]]; then
            local key="${option%%=*}"
            local value="${option#*=}"
            case "$key" in
                ws2812b_enable)
                    cmake_args+=("-DWS2812B_ENABLE=${value}")
                    ;;
                swd_enable)
                    cmake_args+=("-DSWD_ENABLE=${value}")
                    ;;
                custom_langs)
                    cmake_args+=("-DCUSTOM_MULTI_LANGS=${value}")
                    ;;
                *)
                    cmake_args+=("-D${key}=${value}")
                    ;;
            esac
        fi
    done

    # Configure with CMake - point to source directory
    echo "  Configuring with CMake..."
    if [ "$VERBOSE" = true ]; then
        cmake ../../.. "${cmake_args[@]}"
    else
        cmake ../../.. "${cmake_args[@]}" >/dev/null 2>&1
    fi

    if [ $? -ne 0 ]; then
        cd ..
        forceExit "CMake configuration failed for ${model}"
    fi

    # Build the firmware
    local target="firmware-${language}"
    local build_args=()

    if [ "$VERBOSE" = true ]; then
        build_args+=("--verbose")
    fi

    echo "  Building target: ${target}..."
    if [ "$VERBOSE" = true ]; then
        cmake --build . --target "${target}" -j "${JOBS}" "${build_args[@]}"
    else
        cmake --build . --target "${target}" -j "${JOBS}" "${build_args[@]}" >/dev/null 2>&1
    fi

    if [ $? -ne 0 ]; then
        cd ..
        forceExit "Build failed for ${model} with ${language}"
    fi

    # Copy firmware files to unified Hexfile directory
    mkdir -p "../../../Hexfile"
    if [[ -d "Hexfile" ]]; then
        cp -f Hexfile/*.hex ../../../Hexfile/ 2>/dev/null || true
        cp -f Hexfile/*.bin ../../../Hexfile/ 2>/dev/null || true
        cp -f Hexfile/*.dfu ../../../Hexfile/ 2>/dev/null || true
    fi

    cd ../../..
    checkLastCommand
}

# Parse command line arguments
declare -a margs=()
declare -a largs=()
declare -a oargs=()

while getopts "hl:m:o:t:cvj:" option; do
    case "${option}" in
    h)
        usage
        ;;
    l)
        IFS=' ' read -r -a largs <<<"${OPTARG}"
        ;;
    m)
        IFS=' ' read -r -a margs <<<"${OPTARG}"
        ;;
    o)
        IFS=' ' read -r -a oargs <<< "${OPTARG}"
        ;;
    t)
        BUILD_TYPE="${OPTARG}"
        ;;
    c)
        CLEAN=true
        ;;
    v)
        VERBOSE=true
        ;;
    j)
        JOBS="${OPTARG}"
        ;;
    *)
        usage
        ;;
    esac
done
shift $((OPTIND - 1))

builder_info

# Validate build type
case "$BUILD_TYPE" in
    Debug|Release|RelWithDebInfo|MinSizeRel)
        ;;
    *)
        forceExit "Invalid build type: $BUILD_TYPE. Valid options: Debug, Release, RelWithDebInfo, MinSizeRel"
        ;;
esac

# Check if languages are available
if [ ${#AVAILABLE_LANGUAGES[@]} -eq 0 ]; then
    forceExit "No translation files found in $TRANSLATION_DIR"
fi

# Clean if requested
if [ "$CLEAN" = true ]; then
    clean_all
    echo -e "${BLUE}********************************************${NC}"
fi

# Process language arguments
echo -n "Available languages: "
echo "${AVAILABLE_LANGUAGES[*]}"
echo -n "Requested languages: "
if ((${#largs[@]})); then
    for i in "${largs[@]}"; do
        i=$(echo "${i}" | tr '[:lower:]' '[:upper:]')
        if isInArray "$i" "${AVAILABLE_LANGUAGES[@]}"; then
            echo -n "$i "
            BUILD_LANGUAGES+=("$i")
        else
            forceExit "Language '$i' is unknown. Available languages: ${AVAILABLE_LANGUAGES[*]}"
        fi
    done
    echo ""
fi

if [ -z "${BUILD_LANGUAGES[*]}" ]; then
    echo "No custom languages selected."
    echo "Building: [ALL LANGUAGES]"
    BUILD_LANGUAGES+=("${AVAILABLE_LANGUAGES[@]}")
fi
echo -e "${BLUE}********************************************${NC}"

# Process model arguments
echo -n "Available models: "
echo "${AVAILABLE_MODELS[*]}"
echo -n "Requested models: "
if ((${#margs[@]})); then
    for i in "${margs[@]}"; do
        # Handle case sensitivity for special models
        if [[ "$i" != "Pinecil" ]] && [[ "$i" != "Pinecilv2" ]]; then
            i=$(echo "${i}" | tr '[:lower:]' '[:upper:]')
        fi

        if isInArray "$i" "${AVAILABLE_MODELS[@]}"; then
            echo -n "$i "
            BUILD_MODELS+=("$i")
        else
            forceExit "Model '$i' is unknown. Available models: ${AVAILABLE_MODELS[*]}"
        fi
    done
    echo ""
fi

if [ -z "${BUILD_MODELS[*]}" ]; then
    echo "No custom models selected."
    echo "Building: [ALL MODELS]"
    BUILD_MODELS+=("${AVAILABLE_MODELS[@]}")
fi

echo -e "${BLUE}********************************************${NC}"

# Process options
echo -n "Requested options: "
if ((${#oargs[@]})); then
    for i in "${oargs[@]}"; do
        echo -n "$i "
        OPTIONS+=("$i")
    done
    echo ""
else
    echo "None"
fi

echo -e "${BLUE}********************************************${NC}"

# Show build configuration
echo -e "${BLUE}Build Configuration:${NC}"
echo "  Build Type: $BUILD_TYPE"
echo "  Jobs: $JOBS"
echo "  Verbose: $VERBOSE"
echo "  Models: ${BUILD_MODELS[*]}"
echo "  Languages: ${BUILD_LANGUAGES[*]}"
if ((${#OPTIONS[@]})); then
    echo "  Options: ${OPTIONS[*]}"
fi
echo -e "${BLUE}********************************************${NC}"

# Start building
if [ ${#BUILD_LANGUAGES[@]} -gt 0 ] && [ ${#BUILD_MODELS[@]} -gt 0 ]; then
    echo -e "${YELLOW}Starting build process...${NC}"

    # Create unified Hexfile directory
    mkdir -p Hexfile

    for model in "${BUILD_MODELS[@]}"; do
        for language in "${BUILD_LANGUAGES[@]}"; do
            build_firmware "$model" "$language"
        done
    done

    echo -e "${GREEN}********************************************${NC}"
    echo -e "${GREEN} -- All firmwares successfully generated --${NC}"
    echo -e "${BLUE}********************************************${NC}"

    # Show generated files
    echo -e "${BLUE}Generated firmware files:${NC}"
    if [[ -d "Hexfile" ]] && [[ -n "$(ls -A Hexfile/ 2>/dev/null)" ]]; then
        find Hexfile/ -name "*.hex" -o -name "*.bin" -o -name "*.dfu" | sort | while read -r file; do
            size=$(stat -c%s "$file" 2>/dev/null || echo "unknown")
            echo "  $(basename "$file") (${size} bytes)"
        done
    else
        echo "  No firmware files found in Hexfile/"
    fi

    echo -e "${BLUE}********************************************${NC}"
    echo -e "${BLUE}Build artifacts organized in Objects/<model>/ directories${NC}"
    echo -e "${GREEN}End...${NC}"
else
    forceExit "Nothing to build (no model or language specified)"
fi
