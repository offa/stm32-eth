#!/bin/bash

set -ex

## Install GSL
GSL_VERSION=master
wget https://github.com/Microsoft/GSL/archive/${GSL_VERSION}.tar.gz -O gsl.tar.gz
tar -xzf gsl.tar.gz
mkdir dependencies
mv GSL-* dependencies/gsl


if [ "$CXX" != "arm-none-eabi-g++" ]; then
    ## Install CppUTest
    CPPUTEST_VERSION=master
    CPPUTEST=cpputest-${CPPUTEST_VERSION}

    BUILD_FLAGS="-DC++11=ON -DTESTS=OFF"

    if [[ "$CXX" == clang* ]]; then
        BUILD_FLAGS="$BUILD_FLAGS -DCMAKE_CXX_FLAGS=-stdlib=libc++" 
    fi


    wget https://github.com/offa/cpputest/archive/${CPPUTEST_VERSION}.tar.gz
    tar -xzf ${CPPUTEST_VERSION}.tar.gz
    pushd ${CPPUTEST}
    mkdir _build && cd _build
    cmake $BUILD_FLAGS ..
    make -j4 && sudo make install
    popd
fi
