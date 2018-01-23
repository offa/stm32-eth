
add_subdirectory(spi)
add_subdirectory(w5100)

add_cpp_library(socket Socket.cpp)
target_link_libraries(socket PUBLIC GSL::GSL)

add_cpp_library(platform-stm32 PlatformStm32.cpp)


add_cpp_library(eth-stm32)
target_link_libraries(eth-stm32 PUBLIC socket w5100device spiwriter platform-stm32)
add_size_target(eth-stm32)

