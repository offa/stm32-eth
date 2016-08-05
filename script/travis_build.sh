#!/bin/bash

set -ex

mkdir build && cd build

if [ "$CXX" == "arm-none-eabi-g++" ]; then
    cmake -DUNITTEST=OFF -DCMAKE_TOOLCHAIN_FILE=../arm-embedded-toolchain.cmake ..
    make
else
    cmake -DUNITTEST_VERBOSE=ON ..
    make
    make unittest
fi

