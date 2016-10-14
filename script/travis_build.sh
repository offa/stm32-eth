#!/bin/bash

set -ex

if [ -v GSL_INCLUDE_DIR ]; then
    echo "GSL: ${GSL_INCLUDE_DIR}"
else
    export GSL_INCLUDE_DIR=dependencies/gsl
fi


mkdir build && cd build

if [ "$CXX" == "arm-none-eabi-g++" ]; then
    cmake -DUNITTEST=OFF \
            -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
            -DINTEGRATION_TEST=ON \
            -DCMAKE_TOOLCHAIN_FILE=../arm-embedded-toolchain.cmake \
            ..
    make

    make eth-stm32.size
else
    cmake -DUNITTEST_VERBOSE=ON ..
    make
    make unittest
fi

