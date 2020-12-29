#!/bin/bash
set -e
# Setup shell file to setup the environment on an ubuntu machine
sudo apt-get update 
sudo apt-get install -y make bzip2 git python3 wget 
sudo mkdir -p /build
cd /build

#Download source files to cache folder
mkdir -p /build/cache
cd /build/cache/
if md5sum -c /build/source/ci/gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2.md5; then
    echo "Good MD5 ARM"
else
    echo "ARM MD5 Mismatch, downloading fresh"
    sudo wget  https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu-rm/9-2020q2/gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2 -O gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2
fi

if md5sum -c /build/source/ci/nuclei_riscv_newlibc_prebuilt_linux64_2020.08.tar.bz2.md5; then
    echo "Good MD5 RISCV"
else
    echo "RISCV MD5 Mismatch, downloading fresh"
    sudo wget  https://github.com/Ralim/nuclei-compiler/releases/download/2020.08/nuclei_riscv_newlibc_prebuilt_linux64_2020.08.tar.bz2 -O nuclei_riscv_newlibc_prebuilt_linux64_2020.08.tar.bz2
fi

echo "Extracting compilers"
sudo tar -xj -f gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2 -C /build/
sudo tar -xj -f nuclei_riscv_newlibc_prebuilt_linux64_2020.08.tar.bz2 -C /build/

echo "Link into PATH"

sudo ln -s /build/gcc-arm-none-eabi-9-2020-q2-update/bin/* /usr/local/bin
sudo ln -s /build/gcc/bin/* /usr/local/bin
