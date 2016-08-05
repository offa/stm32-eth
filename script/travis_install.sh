#!/bin/bash

set -ex

if [ "$CXX" == "arm-none-eabi-g++" ]; then
    ## Install Gcc (Arm)
    sudo add-apt-repository ppa:team-gcc-arm-embedded/ppa -y
    sudo apt-get -qq update
    sudo apt-get install -y gcc-arm-embedded
else
    ## Install CppUTest
    export CPPUTEST_VERSION=3.8
    export CPPUTEST=cpputest-${CPPUTEST_VERSION}

    wget https://github.com/cpputest/cpputest/releases/download/v${CPPUTEST_VERSION}/${CPPUTEST}.tar.gz
    tar -xzvf *.tar.gz
    cd ${CPPUTEST}
    mkdir _build && cd _build
    cmake -DC++11=ON -DTESTS=OFF ..
    make -j4 && sudo make install
    cd ../..
fi
