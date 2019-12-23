#!/usr/bin/env bash
set -e
set -u

mkdir -p /build/ci/artefacts

# Build STM code
cd /build/source/workspace/TS100/
bash ./build.sh || exit 1
echo "All Firmware built"
# Copy out all the final resulting files we would like to store for the next op
cp -r /build/source/workspace/TS100/Hexfile/*.hex  /build/ci/artefacts/
cp -r /build/source/workspace/TS100/Hexfile/*.bin  /build/ci/artefacts/
