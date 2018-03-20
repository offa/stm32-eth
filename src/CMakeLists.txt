
add_subdirectory(spi)
add_subdirectory(w5100)

add_cpp_library(stm32-socket OBJECT Socket.cpp)
target_include_directories(stm32-socket PRIVATE $<TARGET_PROPERTY:GSL::GSL,INTERFACE_INCLUDE_DIRECTORIES>)

add_cpp_library(stm32-platform OBJECT PlatformStm32.cpp)


add_library(eth-stm32 $<TARGET_OBJECTS:stm32-socket>
                    $<TARGET_OBJECTS:stm32-w5100device>
                    $<TARGET_OBJECTS:stm32-spiwriter>
                    $<TARGET_OBJECTS:stm32-platform>
                    )
add_size_target(eth-stm32)



install(EXPORT eth-stm32-config DESTINATION share/stm32-eth/cmake)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/include/" DESTINATION include)
install(TARGETS eth-stm32 EXPORT eth-stm32-config DESTINATION lib)
