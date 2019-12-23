FROM ubuntu:rolling
MAINTAINER Ben V. Brown <ralim@ralimtek.com>

WORKDIR /build
# Setup the ARM GCC toolchain

# Install any needed packages specified in requirements.txt
RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install -y \
      make \
      git \
      bzip2 \
	    git \
	    golang \
      python3 \
      wget && \
    apt-get clean && \
    wget -qO- https://developer.arm.com/-/media/Files/downloads/gnu-rm/9-2019q4/RC2.1/gcc-arm-none-eabi-9-2019-q4-major-x86_64-linux.tar.bz2 | tar -xj

# Add compiler to the path

ENV PATH "/build/gcc-arm-none-eabi-9-2019-q4-major/bin:$PATH"
# Get the github release tool
RUN go get -u github.com/aktau/github-release
COPY . /build/source
COPY ./ci /build/ci
