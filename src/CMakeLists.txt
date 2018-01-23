
add_subdirectory(spi)
add_subdirectory(w5100)

add_cpp_library(socket Socket.cpp)
target_link_libraries(socket PUBLIC GSL::GSL)

add_cpp_library(platform-stm32 OBJECT PlatformStm32.cpp)


add_cpp_library(eth-stm32
                        $<TARGET_OBJECTS:spiwriter>
                        $<TARGET_OBJECTS:platform-stm32>
                        )
target_link_libraries(eth-stm32 socket w5100device)
add_size_target(eth-stm32)

