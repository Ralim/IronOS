#!/bin/bash
set -e
# Setup shell file to setup the environment on an ubuntu machine
sudo apt-get update && sudo apt-get install -y make bzip2 git python3 python3-pip wget 
python3 -m pip install bdflib black flake8
sudo mkdir -p /build
cd /build

#Download source files to cache folder
# Remember if these are updated, you need to update the md5 file respectively
# Github checks out under $GITHUB_WORKSPACE
MDPATH=${GITHUB_WORKSPACE:-/build/source/}
sudo mkdir -p /build/cache
cd /build/cache/
if md5sum -c $MDPATH/ci/gcc-arm-none-eabi-10-2020-q4-major-x86_64-linux.tar.bz2.md5; then
    echo "Good MD5 ARM"
else
    echo "ARM MD5 Mismatch, downloading fresh"
    sudo wget -q "https://developer.arm.com/-/media/Files/downloads/gnu-rm/10-2020q4/gcc-arm-none-eabi-10-2020-q4-major-x86_64-linux.tar.bz2" -O gcc-arm-none-eabi-10-2020-q4-major-x86_64-linux.tar.bz2
    rm -rf $MDPATH/ci/gcc-arm*.bz2 || true
fi

if md5sum -c $MDPATH/ci/nuclei_riscv_newlibc_prebuilt_linux64_2020.08.tar.bz2.md5; then
    echo "Good MD5 RISCV"
else
    echo "RISCV MD5 Mismatch, downloading fresh"
    sudo wget -q "https://github.com/Ralim/nuclei-compiler/releases/download/2020.08/nuclei_riscv_newlibc_prebuilt_linux64_2020.08.tar.bz2" -O nuclei_riscv_newlibc_prebuilt_linux64_2020.08.tar.bz2
    rm -rf $MDPATH/ci/nuclei*.bz2 || true
fi

echo "Extracting compilers"
sudo tar -xj -f gcc-arm-none-eabi-10-2020-q4-major-x86_64-linux.tar.bz2 -C /build/
sudo tar -xj -f nuclei_riscv_newlibc_prebuilt_linux64_2020.08.tar.bz2 -C /build/

echo "Link into PATH"

sudo ln -s /build/gcc-arm-none-eabi-10-2020-q4-major/bin/* /usr/local/bin
sudo ln -s /build/gcc/bin/* /usr/local/bin
