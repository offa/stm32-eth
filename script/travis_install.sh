#!/bin/bash

set -ex

if [ "$CXX" == "arm-none-eabi-g++" ]; then
    ## Install Gcc (Arm)
    sudo add-apt-repository ppa:team-gcc-arm-embedded/ppa -y
    sudo apt-get -qq update
    sudo apt-get install -y gcc-arm-embedded
else
    ## Install CppUTest
    CPPUTEST_VERSION=3.8
    CPPUTEST=cpputest-${CPPUTEST_VERSION}

    BUILD_FLAGS="-DC++11=ON -DTESTS=OFF"

    if [[ "$CXX" == clang* ]]; then
        BUILD_FLAGS="$BUILD_FLAGS -DCMAKE_CXX_FLAGS=-stdlib=libc++" 
    fi

    wget https://github.com/cpputest/cpputest/releases/download/v${CPPUTEST_VERSION}/${CPPUTEST}.tar.gz
    tar -xzvf *.tar.gz
    cd ${CPPUTEST}
    mkdir _build && cd _build
    cmake $BUILD_FLAGS ..
    make -j4 && sudo make install
    cd ../..
fi
