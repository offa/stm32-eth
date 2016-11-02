#!/bin/bash

set -ex

BUILD_DIR=build


mkdir ${BUILD_DIR} && cd ${BUILD_DIR}

cmake -DUNITTEST_JUNIT=ON -DCOVERAGE=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
make

## Coverage 
make coverage-clean
make unittest
make coverage

python ${LCOV_COBERTURA}/lcov_cobertura.py coverage/stm32-eth.info --base-dir ../src --output coverage/coverage.xml --demangle
cd -


## CppCheck
cppcheck -v --error-exitcode=1 \
            --check-config \
            --check-library \
            --report-progress \
            --xml-version=2 \
            --enable=all \
            -I include \
            --suppress=missingInclude \
            -i system/src \
            --project="${BUILD_DIR}/compile_commands.json" \
            src include \
            2> cppcheck-result.xml

