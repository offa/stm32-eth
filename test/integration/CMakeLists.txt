
add_cpp_executable(stm32-eth-it Main.cpp)
target_link_libraries(stm32-eth-it
                        PRIVATE
                            stm32-eth
                            build-libs
                            system-libs
                        )
target_include_directories(stm32-eth-it PRIVATE $<TARGET_PROPERTY:GSL::GSL,INTERFACE_INCLUDE_DIRECTORIES>)

add_cpp_executable(stm32-eth-client-it ClientMain.cpp)
target_link_libraries(stm32-eth-client-it
                        PRIVATE
                            stm32-eth
                            build-libs
                            system-libs
                        )
target_include_directories(stm32-eth-client-it PRIVATE $<TARGET_PROPERTY:GSL::GSL,INTERFACE_INCLUDE_DIRECTORIES>)


add_hex_target(stm32-eth-it)
add_bin_target(stm32-eth-it)
add_size_target(stm32-eth-it)

add_hex_target(stm32-eth-client-it)
add_bin_target(stm32-eth-client-it)
add_size_target(stm32-eth-client-it)


