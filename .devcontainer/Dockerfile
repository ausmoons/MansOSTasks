FROM mcr.microsoft.com/devcontainers/base:ubuntu-22.04

# Install all dependencies in a single layer to optimize build time
RUN apt-get update && apt-get install -y \
    gcc-msp430 \
    binutils-msp430 \
    msp430-libc \
    msp430mcu \
    gcc-avr \
    binutils-avr \
    avr-libc \
    python3 \
    python3-pip \
    python3-ply \
    python3-serial \
    make \
    git \
    && pip3 install --no-cache-dir pyserial \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/* \
    && ln -s /usr/bin/python3 /usr/local/bin/python \
    && usermod -a -G dialout vscode

# Set working directory
WORKDIR /workspace
