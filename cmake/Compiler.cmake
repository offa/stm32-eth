macro(add_c_flag)
    foreach(flag ${ARGN})
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${flag}")
    endforeach()
endmacro()

macro(add_cxx_flag)
    foreach(flag ${ARGN})
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}")
    endforeach()
endmacro()

macro(add_compiler_flag)
    foreach(flag ${ARGN})
        add_c_flag(${flag})
        add_cxx_flag(${flag})
    endforeach()
endmacro()

macro(add_linker_flag)
    foreach(flag ${ARGN})
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${flag}")
    endforeach()
endmacro()


function(add_hex_target targetname)
    add_custom_target(${targetname}.hex DEPENDS ${targetname}
                                    COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${targetname}> ${targetname}.hex
                                    VERBATIM
                                    )
endfunction()


function(add_bin_target targetname)
    add_custom_target(${targetname}.bin DEPENDS ${targetname}
                                    COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${targetname}> ${targetname}.bin
                                    VERBATIM
                                    )
endfunction()


find_program(SIZE_EXE size)

function(add_size_target targetname)
    if( NOT SIZE_EXE )
        message(WARNING "'size' not found!")
    else()
        add_custom_target(${targetname}.size DEPENDS ${targetname}
                                        COMMAND ${SIZE_EXE} -t --format=berkeley $<TARGET_FILE:${targetname}>
                                        VERBATIM
                                        )
    endif()
endfunction()



if( CMAKE_CROSSCOMPILING )
    set(CMAKE_EXECUTABLE_SUFFIX ".elf")
endif()

