#!/bin/bash

set -ex

if [ ! -v LTO_ENABLED ]; then
    LTO_ENABLED=ON
else
    LTO_ENABLED=OFF
fi


if [ ! -v GSL_INCLUDE_DIR ]; then
    if [ -d "dependencies/gsl" ]; then
        export GSL_INCLUDE_DIR=dependencies/gsl
    fi
fi


mkdir build && cd build

if [ "$CXX" == "arm-none-eabi-g++" ]; then
    cmake -DUNITTEST=OFF \
            -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
            -DINTEGRATION_TEST=ON \
            -DLTO=${LTO_ENABLED} \
            -DCMAKE_TOOLCHAIN_FILE=../arm-embedded-toolchain.cmake \
            ..
    make

    if [ "${LTO_ENABLED}" != "ON" ]; then
        make eth-stm32.size
    fi
else
    cmake -DUNITTEST_VERBOSE=ON -DCMAKE_BUILD_TYPE=Release ..
    make
    make unittest
fi

