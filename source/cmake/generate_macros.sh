#!/bin/bash

# Script to generate macros.txt for IronOS translation system
# Usage: generate_macros.sh <compiler> <output_dir> <source_dir> <bsp_dir> <defines...> -- <cpu_flags...>

set -e

if [ $# -lt 4 ]; then
    echo "Usage: $0 <compiler> <output_dir> <source_dir> <bsp_dir> <defines...> -- <cpu_flags...>"
    exit 1
fi

COMPILER="$1"
OUTPUT_DIR="$2"
SOURCE_DIR="$3"
BSP_DIR="$4"
shift 4

# Parse arguments - everything before "--" is defines, everything after is CPU flags
DEFINES=()
CPU_FLAGS=()
parsing_defines=true

for arg in "$@"; do
    if [ "$arg" = "--" ]; then
        parsing_defines=false
        continue
    fi

    if [ "$parsing_defines" = true ]; then
        DEFINES+=("$arg")
    else
        CPU_FLAGS+=("$arg")
    fi
done

# Create output directory
mkdir -p "$OUTPUT_DIR"

# Create input file
echo "#include <configuration.h>" > "$OUTPUT_DIR/macros_input.c"

# Generate macros.txt
# Check if BSP_DIR is already absolute (starts with /)
if [[ "$BSP_DIR" = /* ]]; then
    BSP_INCLUDE_DIR="$BSP_DIR"
else
    BSP_INCLUDE_DIR="$SOURCE_DIR/$BSP_DIR"
fi

"$COMPILER" -dM -E \
    "${DEFINES[@]}" \
    "${CPU_FLAGS[@]}" \
    -I"$SOURCE_DIR/Core/Inc" \
    -I"$BSP_INCLUDE_DIR" \
    "$OUTPUT_DIR/macros_input.c" > "$OUTPUT_DIR/macros.txt"

echo "Generated macros.txt successfully"
