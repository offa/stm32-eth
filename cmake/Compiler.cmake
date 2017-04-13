macro(add_c_flag)
    foreach(_flag ${ARGN})
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${_flag}")
    endforeach()
endmacro()

macro(add_cxx_flag)
    foreach(_flag ${ARGN})
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${_flag}")
    endforeach()
endmacro()

macro(add_compiler_flag)
    foreach(_flag ${ARGN})
        add_c_flag(${_flag})
        add_cxx_flag(${_flag})
    endforeach()
endmacro()

macro(add_linker_flag)
    foreach(_flag ${ARGN})
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${_flag}")
    endforeach()
endmacro()


macro(add_hex_target _target)
    add_custom_target(${_target}.hex DEPENDS ${_target}
                                    COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${_target}> ${_target}.hex
                                    VERBATIM
                                    )
endmacro()


macro(add_bin_target _target)
    add_custom_target(${_target}.bin DEPENDS ${_target}
                                    COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${_target}> ${_target}.bin
                                    VERBATIM
                                    )
endmacro()


find_program(SIZE_EXE size)

macro(add_size_target _target)
    if( NOT SIZE_EXE )
        message(WARNING "'size' not found!")
    else()
        add_custom_target(${_target}.size DEPENDS ${_target}
                                        COMMAND ${SIZE_EXE} -t --format=berkeley $<TARGET_FILE:${_target}>
                                        VERBATIM
                                        )
    endif()
endmacro()



if( CMAKE_CROSSCOMPILING )
    set(CMAKE_EXECUTABLE_SUFFIX ".elf")
endif()

