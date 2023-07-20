#!/bin/sh
set -e
set -u

# Init vars & prepare dir
dir_ci="/build/ci"
dir_artefacts="${dir_ci}/artefacts"
dir_ironos="/build/ironos"
dir_source="${dir_ironos}/source"
mkdir -p "${dir_artefacts}"

# Build firmware
cd "${dir_source}"
bash ./build.sh || exit 1
echo "All Firmware built"

# Copy out all the final resulting files we would like to store for the next op
cp -r "${dir_source}"/Hexfile/{*.hex,*.bin}  "${dir_artefacts}"
