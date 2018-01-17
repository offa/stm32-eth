
add_executable(eth-stm32-it Main.cpp)
target_link_libraries(eth-stm32-it
                        PRIVATE
                            eth-stm32
                            build-libs
                        PUBLIC
                           system-libs
                        )

add_executable(eth-stm32-client-it ClientMain.cpp)
target_link_libraries(eth-stm32-client-it
                        PRIVATE
                            eth-stm32
                            build-libs
                        PUBLIC
                            system-libs
                        )


add_hex_target(eth-stm32-it)
add_bin_target(eth-stm32-it)
add_size_target(eth-stm32-it)

add_hex_target(eth-stm32-client-it)
add_bin_target(eth-stm32-client-it)
add_size_target(eth-stm32-client-it)


