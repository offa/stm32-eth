
add_subdirectory(spi)
add_subdirectory(w5100)

add_cpp_library(stm32-socket Socket.cpp)
target_link_libraries(stm32-socket PRIVATE GSL::GSL)

add_cpp_library(stm32-platform PlatformStm32.cpp)

add_library(eth-stm32 INTERFACE)
target_compile_options(eth-stm32 INTERFACE ${flags_common} ${flags_cpp})
target_link_libraries(eth-stm32 INTERFACE
                        stm32-socket
                        stm32-w5100device
                        stm32-spiwriter
                        stm32-platform
                        )

#add_cpp_library(eth-stm32 INTERFACE)
#target_link_libraries(eth-stm32 PUBLIC
                        #stm32-socket
                        #stm32-w5100device
                        #stm32-spiwriter
                        #stm32-platform
                        #)
#add_size_target(eth-stm32)

