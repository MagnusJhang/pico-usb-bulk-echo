#!/bin/bash
BOARD=${1:-pico}
TOOLCHAIN_DIR="$PWD/gcc-arm-none-eabi"
if ! command -v arm-none-eabi-gcc &> /dev/null; then
    if [ -d "$TOOLCHAIN_DIR" ]; then export PATH="$TOOLCHAIN_DIR/bin:$PATH";
    else
        echo "Downloading Toolchain..."
        URL="https://developer.arm.com/-/media/Files/downloads/gnu/13.2.rel1/binrel/arm-gnu-toolchain-13.2.rel1-x86_64-arm-none-eabi.tar.xz"
        wget -q --show-progress $URL -O toolchain.tar.xz
        mkdir -p "$TOOLCHAIN_DIR" && tar -xJf toolchain.tar.xz -C "$TOOLCHAIN_DIR" --strip-components=1 && rm toolchain.tar.xz
        export PATH="$TOOLCHAIN_DIR/bin:$PATH"
    fi
fi
export PICO_SDK_PATH="$(pwd)/pico-sdk"
BUILD_DIR="build_$BOARD"
rm -rf $BUILD_DIR && mkdir -p $BUILD_DIR && cd $BUILD_DIR && cmake -DPICO_BOARD=$BOARD .. && make -j$(nproc)
