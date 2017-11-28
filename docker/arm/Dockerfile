FROM registry.gitlab.com/offa/docker-images/base-cpp:latest

RUN add-apt-repository ppa:team-gcc-arm-embedded/ppa && \
    apt-get update && \
    apt-get install -y --no-install-recommends gcc-arm-embedded && \
    mkdir deps && cd deps && \
    git clone --depth=1 https://github.com/Microsoft/GSL.git gsl && \
    cd gsl && mkdir build && cd build && \
    cmake -DGSL_TEST=OFF .. && make && make install && \
    cd ../../.. && rm -rf deps

