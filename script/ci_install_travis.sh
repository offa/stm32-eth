#!/bin/bash

set -ex

CMAKE_VERSION_FULL="${CMAKE_VERSION}.0"

## Install CMake
mkdir -p "${DEPENDENCY_DIR}"
cd "${DEPENDENCY_DIR}"

if [[ ! -d "$(ls -A ${DEPENDENCY_DIR}/cmake)" ]]; then
    CMAKE_URL="https://cmake.org/files/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION_FULL}-Linux-x86_64.tar.gz"
    mkdir -p cmake && travis_retry wget --no-check-certificate --quiet -O - "${CMAKE_URL}" | tar --strip-components=1 -xz -C cmake
fi

