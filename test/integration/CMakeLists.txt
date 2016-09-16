
add_executable(IntegrationTest Main.cpp
                        $<TARGET_OBJECTS:hal>
                        $<TARGET_OBJECTS:cmsis>
                        $<TARGET_OBJECTS:newlib>
                        $<TARGET_OBJECTS:cortexm>
                        $<TARGET_OBJECTS:misc>
                        $<TARGET_OBJECTS:trace>
                        )
target_link_libraries(IntegrationTest eth-stm32)


add_hex_target(IntegrationTest)
add_bin_target(IntegrationTest)
add_size_target(IntegrationTest)

