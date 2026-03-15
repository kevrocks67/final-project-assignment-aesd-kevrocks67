# Use the non-ancient 24.04 image
FROM ubuntu:24.04

# Suppress interactive prompts
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    doxygen \
    graphviz \
    lcov \
    linux-libc-dev \
    ruby \
    check \
    cpio \
    openssl \
    gcc-arm-linux-gnueabihf \
    && rm -rf /var/lib/apt/lists/*

# Set the working directory
WORKDIR /workspace
