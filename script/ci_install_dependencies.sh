#!/bin/bash

set -ex

if [[ "${CXX}" == clang* ]]
then
    export CXXFLAGS="-stdlib=libc++"
fi


git clone --branch=master --depth=1 https://github.com/cpputest/cpputest.git cpputest
mkdir -p cpputest/_build
cd cpputest/_build
cmake -DCMAKE_CXX_STANDARD=20 -DTESTS=OFF -DMEMORY_LEAK_DETECTION=OFF ..
make -j install
cd ../..

