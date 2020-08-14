function(link_to_obj targetName)
    set(multiValueArgs DEFAULT SYSTEM)
    cmake_parse_arguments(LINK_OPTION "" "" "${multiValueArgs}" ${ARGN})

    foreach(lib IN LISTS LINK_OPTION_DEFAULT)
        target_include_directories(${targetName} PRIVATE $<TARGET_PROPERTY:${lib},INTERFACE_INCLUDE_DIRECTORIES>)
        target_compile_definitions(${targetName} PRIVATE $<TARGET_PROPERTY:${lib},INTERFACE_COMPILE_DEFINITIONS>)
    endforeach()
    foreach(lib IN LISTS LINK_OPTION_SYSTEM)
        target_include_directories(${targetName} SYSTEM PRIVATE $<TARGET_PROPERTY:${lib},INTERFACE_INCLUDE_DIRECTORIES>)
        target_compile_definitions(${targetName} PRIVATE $<TARGET_PROPERTY:${lib},INTERFACE_COMPILE_DEFINITIONS>)
    endforeach()
endfunction()


add_subdirectory(spi)
add_subdirectory(w5100)

add_cpp_library(stm32-socket OBJECT Socket.cpp)
link_to_obj(stm32-socket DEFAULT Microsoft.GSL::Microsoft.GSL SYSTEM stm32hal-api)

add_cpp_library(stm32-platform OBJECT PlatformStm32.cpp)
link_to_obj(stm32-platform SYSTEM stm32hal-api)


add_library(stm32-eth $<TARGET_OBJECTS:stm32-socket>
                    $<TARGET_OBJECTS:stm32-w5100device>
                    $<TARGET_OBJECTS:stm32-spiwriter>
                    $<TARGET_OBJECTS:stm32-platform>
                    )
add_utility_target(stm32-eth SIZE)



install(EXPORT stm32-eth-config DESTINATION share/stm32-eth/cmake)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/include/" DESTINATION include)
install(TARGETS stm32-eth EXPORT stm32-eth-config DESTINATION lib)
