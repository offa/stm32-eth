#!/bin/bash

set -ex

LTO_ENABLED=${LTO_ENABLED:=OFF}
BUILD_TYPE=${BUILD_TYPE:=Release}

if [[ ! -v GSL_INCLUDE_DIR ]]
then
    export GSL_INCLUDE_DIR=/usr/local/include
fi

if [[ "${CXX}" == clang* ]]
then
    export CXXFLAGS="-stdlib=libc++"
fi



mkdir build && cd build

if [[ "$CXX" == "arm-none-eabi-g++" ]]
then
    cmake -DUNITTEST=OFF \
            -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
            -DINTEGRATION_TEST=ON \
            -DLTO=${LTO_ENABLED} \
            -DCMAKE_TOOLCHAIN_FILE=../arm-embedded-toolchain.cmake \
            ..
    make

    if [[ "${LTO_ENABLED}" != "ON" ]]
    then
        make eth-stm32.size
    fi
else
    cmake -DUNITTEST_VERBOSE=ON -DCMAKE_BUILD_TYPE=${BUILD_TYPE} ..
    make
    make unittest
fi

