
add_library(socket OBJECT Socket.cpp)
add_library(w5100device OBJECT W5100Device.cpp)
add_library(spiwriter OBJECT SpiWriter.cpp)
add_library(platform-stm32 OBJECT PlatformStm32.cpp)

add_library(eth-stm32 $<TARGET_OBJECTS:socket>
                        $<TARGET_OBJECTS:w5100device>
                        $<TARGET_OBJECTS:spiwriter>
                        $<TARGET_OBJECTS:platform-stm32>
                        )
add_size_target(eth-stm32)

