
add_executable(eth-stm32-it Main.cpp
                        $<TARGET_OBJECTS:hal>
                        $<TARGET_OBJECTS:cmsis>
                        $<TARGET_OBJECTS:newlib>
                        $<TARGET_OBJECTS:cortexm>
                        $<TARGET_OBJECTS:misc>
                        $<TARGET_OBJECTS:trace>
                        )
target_link_libraries(eth-stm32-it eth-stm32)

add_executable(eth-stm32-client-it ClientMain.cpp
                        $<TARGET_OBJECTS:hal>
                        $<TARGET_OBJECTS:cmsis>
                        $<TARGET_OBJECTS:newlib>
                        $<TARGET_OBJECTS:cortexm>
                        $<TARGET_OBJECTS:misc>
                        $<TARGET_OBJECTS:trace>
                        )
target_link_libraries(eth-stm32-client-it eth-stm32)


add_hex_target(eth-stm32-it)
add_bin_target(eth-stm32-it)
add_size_target(eth-stm32-it)

add_hex_target(eth-stm32-client-it)
add_bin_target(eth-stm32-client-it)
add_size_target(eth-stm32-client-it)


