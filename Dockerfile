ARG COMPILER

FROM registry.gitlab.com/offa/docker-images/${COMPILER}:stable

ARG COMPILER

RUN mkdir deps && cd deps && \
    git clone --depth=1 https://github.com/Microsoft/GSL.git gsl && \
    cd gsl && mkdir build && cd build && \
    cmake -DGSL_TEST=OFF .. && make && make install && \
    cd ../.. && \
    if [ "${COMPILER}" = "${COMPILER#arm-none-eabi-gcc}" ]; then \
        git clone --branch=latest-passing-build --depth=1 https://github.com/cpputest/cpputest.git cpputest && \
        cd cpputest && mkdir _build && cd _build && \
        case ${CXX} in clang* ) export CXXFLAGS="-stdlib=libc++"; esac; \
        cmake -DCMAKE_CXX_STANDARD=17 -DC++11=ON -DTESTS=OFF -DMEMORY_LEAK_DETECTION=OFF .. && \
        make && make install && \
        cd ../..; \
    fi && \
    cd .. && rm -rf deps
