FROM alpine:3.15
LABEL maintainer="Ben V. Brown <ralim@ralimtek.com>"

WORKDIR /build
# Installing the two compilers, python3, python3 pip, clang format
# Compilders ->gcc-* newlib-*
# Python3 -> py*
# Misc -> findutils make git
# musl-dev is required for the multi lang firmwares
# clang is required for clang-format (for dev)

RUN apk add --no-cache gcc-riscv-none-elf gcc-arm-none-eabi newlib-riscv-none-elf newlib-arm-none-eabi findutils python3 py3-pip make git musl-dev clang bash clang-extra-tools

# Install Python3 packages

RUN python3 -m pip install bdflib black

COPY . /build/source
COPY ./ci /build/ci
