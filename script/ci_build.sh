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




mkdir build && cd build

if [[ "${CXX}" == "arm-none-eabi-g++" ]]
then
    cmake ${BUILD_ARGS} \
            -DUNITTEST=OFF \
            -DINTEGRATION_TEST=ON \
            -DCMAKE_TOOLCHAIN_FILE=../arm-embedded-toolchain.cmake \
            ..
    make

    if [[ "${LTO_ENABLED}" != "ON" ]]
    then
        #make eth-stm32.size
        echo "*** Size of the library target is unsupported at the moment ***"
        # Disabled as not supported at the moment

        make eth-stm32-it.size
        make eth-stm32-client-it.size
    fi
else
    cmake ${BUILD_ARGS} \
            -DUNITTEST_VERBOSE=ON \
            ..
    make
    make unittest
fi

