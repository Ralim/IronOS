FROM ubuntu:20.04
LABEL maintainer="Ben V. Brown <ralim@ralimtek.com>"

WORKDIR /build
# Setup the ARM GCC toolchain

# Install any needed packages specified in requirements.txt
RUN apt-get update && \
  apt-get install -y \
  make \
  bzip2 \
  git \
  python3 \
  wget && \
  apt-get clean
RUN wget -qO- https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu-rm/9-2020q2/gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2 | tar -xj
RUN wget -qO- https://github.com/riscv-mcu/riscv-gnu-toolchain/releases/download/v9.2RC/rv_linux_bare_1908312208.tar.bz2 | tar -xj

# Add compiler to the path
ENV PATH "/build/gcc-arm-none-eabi-9-2020-q2-update/bin:$PATH"
ENV PATH "/build/rv_linux_bare_1908312208/bin/:$PATH"
COPY . /build/source
COPY ./ci /build/ci
