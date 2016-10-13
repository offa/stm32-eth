#!/bin/bash

set -ex

echo "LTO: ${LTO_ENABLED}"

if [ -v LTO_ENABLED ]; then
    LTO_ENABLED=ON
    echo "--> ON"
else
    LTO_ENABLED=OFF
    echo "--> OFF"
fi
echo "LTO: ${LTO_ENABLED}"


mkdir build && cd build

if [ "$CXX" == "arm-none-eabi-g++" ]; then
    cmake -DUNITTEST=OFF -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DINTEGRATION_TEST=ON -DLTO=${LTO_ENABLED} -DCMAKE_TOOLCHAIN_FILE=../arm-embedded-toolchain.cmake ..
    make

    make eth-stm32.size
else
    cmake -DUNITTEST_VERBOSE=ON ..
    make
    make unittest
fi

