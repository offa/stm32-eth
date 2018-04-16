#!/bin/bash

set -ex

LTO_ENABLED=${LTO_ENABLED:=OFF}
BUILD_TYPE=${BUILD_TYPE:=Debug}

BUILD_ARGS="-DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DLTO=${LTO_ENABLED}"

for arg in "$@"
do
    case "${arg}" in
        -asan)
            BUILD_ARGS="${BUILD_ARGS} -DSANITIZER_ASAN=ON"
            ;;
        -ubsan)
            BUILD_ARGS="${BUILD_ARGS} -DSANITIZER_UBSAN=ON"
            ;;
    esac
done



if [[ ! -v GSL_INCLUDE_DIR ]]
then
    export GSL_INCLUDE_DIR=/usr/local/include
fi

if [[ "${CXX}" == clang* ]]
then
    export CXXFLAGS="-stdlib=libc++"
fi




mkdir -p build && cd build

if [[ "${CXX}" == "arm-none-eabi-g++" ]]
then
    cmake ${BUILD_ARGS} \
            -DUNITTEST=OFF \
            -DINTEGRATIONTEST=ON \
            -DCMAKE_TOOLCHAIN_FILE=../arm-embedded-toolchain.cmake \
            ..
    make

    if [[ "${LTO_ENABLED}" != "ON" ]]
    then
        make stm32-eth.size
        make stm32-eth-it.size
        make stm32-eth-client-it.size
    fi
else
    cmake ${BUILD_ARGS} \
            -DUNITTEST_VERBOSE=ON \
            ..
    make
    make unittest
fi

