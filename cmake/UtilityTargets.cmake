include(CMakeParseArguments)

find_program(SIZE_BIN size)


function(add_utility_target targetname)
    set(options BIN_FILE HEX_FILE SIZE)
    cmake_parse_arguments(TARGET_OPTIONS "${options}" "" "" ${ARGN})

    if( TARGET_OPTIONS_BIN_FILE )
        add_custom_target(${targetname}.hex
                            DEPENDS ${targetname}
                            COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${targetname}> ${targetname}.hex
                            VERBATIM
                            )
    endif()

    if( TARGET_OPTIONS_HEX_FILE )
        add_custom_target(${targetname}.bin
                            DEPENDS ${targetname}
                            COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${targetname}> ${targetname}.bin
                            VERBATIM
                            )
    endif()

    if( TARGET_OPTIONS_SIZE )
        if( NOT SIZE_BIN )
            message(WARNING "'size' not found!")
        else()
            add_custom_target(${targetname}.size
                                DEPENDS ${targetname}
                                COMMAND ${SIZE_BIN} -t --format=berkeley $<TARGET_FILE:${targetname}>
                                VERBATIM
                                )
        endif()
    endif()
endfunction()

