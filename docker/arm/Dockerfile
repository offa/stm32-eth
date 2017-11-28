FROM registry.gitlab.com/offa/docker-images/arm-none-eabi-gcc:stable

RUN mkdir deps && cd deps && \
    git clone --depth=1 https://github.com/Microsoft/GSL.git gsl && \
    cd gsl && mkdir build && cd build && \
    cmake -DGSL_TEST=OFF .. && make && make install && \
    cd ../../.. && rm -rf deps

