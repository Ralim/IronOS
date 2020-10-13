#!/bin/bash
set -e
# Setup shell file to setup the environment on an ubuntu machine
sudo apt-get update 
sudo apt-get install -y make bzip2 git python3 wget 
sudo mkdir /build
cd /build

sudo wget -qO- https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu-rm/9-2020q2/gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2 | sudo tar -xj
RUN wget -qO- https://github.com/riscv-mcu/riscv-gnu-toolchain/releases/download/v9.2RC/rv_linux_bare_1908312208.tar.bz2 | tar -xj

# Add compiler to the path

sudo ln -s /build/gcc-arm-none-eabi-9-2020-q2-update/bin/* /usr/local/bin
sudo ln -s /build/rv_linux_bare_1908312208/bin/* /usr/local/bin
