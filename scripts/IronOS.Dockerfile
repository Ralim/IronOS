# Default Reference Distro for development env & deploy:
# * Alpine Linux, version 3.16 *
FROM alpine:3.16
LABEL maintainer="Ben V. Brown <ralim@ralimtek.com>"

# Default current dir when container starts
WORKDIR /build/source

# Installing the two compilers (ARM & RISCV), python3 & pip, clang tools:
## - compilers: gcc-*, newlib-*
## - python3: py*, black (required to check Python code formatting)
## - misc: findutils, make, git, diffutils
## - musl-dev (required for the multi lang firmwares)
## - clang (required for clang-format to check C++ code formatting)

ARG APK_COMPS="gcc-riscv-none-elf gcc-arm-none-eabi newlib-riscv-none-elf \
               newlib-arm-none-eabi"
ARG APK_PYTHON="python3 py3-pip black"
ARG APK_MISC="findutils make git diffutils"
ARG APK_DEV="musl-dev clang bash clang-extra-tools"

# PIP packages
ARG PIP_PKGS='bdflib'

# Install system packages using alpine package manager
RUN apk add --no-cache ${APK_COMPS} ${APK_PYTHON} ${APK_MISC} ${APK_DEV}

# Install Python3 packages as modules using pip
RUN python3 -m pip install ${PIP_PKGS}

# Git trust to avoid related warning
RUN git config --global --add safe.directory /build/source

COPY . /build/source
COPY ./scripts/ci /build/ci
