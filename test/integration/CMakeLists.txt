
add_executable(eth-stm32-it Main.cpp)
target_link_libraries(eth-stm32-it eth-stm32
                                cmsis
                                hal
                                newlib
                                cortexm
                                hw-init
                                trace
                                )

add_executable(eth-stm32-client-it ClientMain.cpp)
target_link_libraries(eth-stm32-client-it eth-stm32
                                        cmsis
                                        hal
                                        newlib
                                        cortexm
                                        hw-init
                                        trace
                                        )


add_hex_target(eth-stm32-it)
add_bin_target(eth-stm32-it)
add_size_target(eth-stm32-it)

add_hex_target(eth-stm32-client-it)
add_bin_target(eth-stm32-client-it)
add_size_target(eth-stm32-client-it)


