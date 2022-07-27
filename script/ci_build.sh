#!/bin/bash

set -ex

LTO_ENABLED=${LTO_ENABLED:=OFF}
BUILD_TYPE=${BUILD_TYPE:=Release}
BUILD_ARGS=("-DCMAKE_BUILD_TYPE=${BUILD_TYPE}" "-DLTO=${LTO_ENABLED}")

for arg in "$@"
do
    case "${arg}" in
        -asan)
            BUILD_ARGS+=("-DSANITIZER_ASAN=ON")
            ;;
        -ubsan)
            BUILD_ARGS+=("-DSANITIZER_UBSAN=ON")
            ;;
        -codeql)
            BUILD_ARGS=("-DCMAKE_BUILD_TYPE=Debug")
            ;;
    esac
done


if [[ "${CXX}" == clang* ]]
then
    export CXXFLAGS="-stdlib=libc++"
fi


mkdir -p build && cd build


if [[ ${COMPILER} == arm-none-eabi-gcc* ]]
then
    cmake "${BUILD_ARGS[@]}" \
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
    cmake "${BUILD_ARGS[@]}" \
            -DUNITTEST_VERBOSE=ON \
            ..
    make
    make unittest
fi
