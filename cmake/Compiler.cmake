

macro(add_common_flag)
    foreach(_flag ${ARGN})
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${_flag}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${_flag}")
    endforeach()
endmacro()


macro(add_cxx_flag)
    foreach(_flag ${ARGN})
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${_flag}")
    endforeach()
endmacro()


macro(add_hex_target _target)
    add_custom_target(${_target}.hex DEPENDS ${_target}
                                    COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${_target}> ${_target}.hex
                                    )
endmacro()


macro(add_bin_target _target)
    add_custom_target(${_target}.bin DEPENDS ${_target}
                                    COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${_target}> ${_target}.bin
                                    )
endmacro()


if( NOT CMAKE_CROSSCOMPILING )
    set(CM4_MACRO_TEST "${CMAKE_MODULE_PATH}/Cm4MacroTest.cpp")

    try_compile(CM4_MACRO_WORKS "${CMAKE_BINARY_DIR}/temp"
                                ${CM4_MACRO_TEST}
                                COMPILE_DEFINITIONS -std=c++14
                                )

     if( NOT CM4_MACRO_WORKS )
        message(STATUS "Compiler needs Workaround for Cortex-M4 macros")
        add_definitions(-DCM4_MACRO_WORKAROUND_NEEDED)
    endif()

endif()
