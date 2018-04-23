function(link_to_obj targetName)
    foreach(lib IN LISTS ARGN)
        target_include_directories(${targetName} SYSTEM PRIVATE $<TARGET_PROPERTY:${lib},INTERFACE_INCLUDE_DIRECTORIES>)
        target_compile_definitions(${targetName} PRIVATE $<TARGET_PROPERTY:${lib},INTERFACE_COMPILE_DEFINITIONS>)
    endforeach()
endfunction()


add_subdirectory(spi)
add_subdirectory(w5100)

add_cpp_library(stm32-socket OBJECT Socket.cpp)
link_to_obj(stm32-socket GSL::GSL stm32hal-api)

add_cpp_library(stm32-platform OBJECT PlatformStm32.cpp)
link_to_obj(stm32-platform stm32hal-api)


add_library(stm32-eth $<TARGET_OBJECTS:stm32-socket>
                    $<TARGET_OBJECTS:stm32-w5100device>
                    $<TARGET_OBJECTS:stm32-spiwriter>
                    $<TARGET_OBJECTS:stm32-platform>
                    )
add_utility_target(stm32-eth SIZE)



install(EXPORT stm32-eth-config DESTINATION share/stm32-eth/cmake)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/include/" DESTINATION include)
install(TARGETS stm32-eth EXPORT stm32-eth-config DESTINATION lib)
