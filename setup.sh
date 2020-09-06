#!/bin/bash
set -e
# Setup shell file to setup the environment on an ubuntu machine
sudo apt-get update
sudo apt-get install -y make bzip2 git python3 wget 
sudo mkdir /build
cd /build

sudo wget -qO- https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu-rm/9-2020q2/gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2 | sudo tar -xj

# Add compiler to the path

sudo ln -s /build/gcc-arm-none-eabi-9-2020-q2-update/bin/* /usr/local/bin
