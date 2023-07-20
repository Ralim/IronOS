#!/bin/sh
set -e
set -u

# Init vars
dir_ci="/build/ci"
dir_ironos="/build/ironos"
dir_source="${dir_ironos}/source"

# Prepare output dir
dir_artefacts="${dir_ci}/artefacts"
mkdir -p "${dir_artefacts}"

# Build firmware
cd "${dir_source}"
bash ./build.sh || exit 1
echo "All Firmware built"

# Copy out all the final resulting files we would like to store for the next op
cp -r "${dir_source}"/Hexfile/*.bin  "${dir_artefacts}"
cp -r "${dir_source}"/Hexfile/*.hex  "${dir_artefacts}"
