#!/bin/bash
set -e
# Setup shell file to setup the environment on an ubuntu machine
apt-get update && \
  apt-get install -y \
  make \
  bzip2 \
  git \
  python3 \
  wget && \
  apt-get clean

wget -qO- https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu-rm/9-2020q2/gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2 | tar -xj

# Add compiler to the path
export PATH=/build/gcc-arm-none-eabi-9-2020-q2-update/bin:$PATH
