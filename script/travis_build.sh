#!/bin/bash

set -ex

export GSL_INCLUDE_DIR=dependencies/gsl


mkdir build && cd build

if [ "$CXX" == "arm-none-eabi-g++" ]; then
    cmake -DUNITTEST=OFF -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DINTEGRATION_TEST=ON -DCMAKE_TOOLCHAIN_FILE=../arm-embedded-toolchain.cmake ..
    make

    make eth-stm32.size
else
    cmake -DUNITTEST_VERBOSE=ON ..
    make
    make unittest
fi

