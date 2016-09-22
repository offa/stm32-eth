#!/bin/bash

set -ex

BUILD_DIR=build

pwd
ls

cppcheck -v --error-exitcode=1 \
            --check-config \
            --check-library \
            --report-progress \
            --xml-version=2 \
            --enable=all \
            -I include \
            --suppress=missingInclude \
            -i system/src \
            src include 2> cppcheck-result.xml
            #--project="${BUILD_DIR}/compile_commands.json" \

