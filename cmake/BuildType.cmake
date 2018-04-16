
macro(enable_build_type type)
    list(APPEND VALID_BUILD_TYPES ${type})
endmacro()

function(set_build_type type)
    list(FIND VALID_BUILD_TYPES ${type} type_found)

    if( NOT ( ${type_found} LESS 0 ) )
        string(TOUPPER ${type} type_name)
        set(BUILD_TYPE_${type_name} TRUE)
    else()
        message(SEND_ERROR "Warning:\tInvalid / unsupported build type: '${CMAKE_BUILD_TYPE}'")
    endif()
endfunction()


enable_build_type("Debug")
enable_build_type("Release")
enable_build_type("RelWithDebInfo")
enable_build_type("MinSizeRel")


if( CMAKE_BUILD_TYPE )
    set_build_type(${CMAKE_BUILD_TYPE})
else()
    set(BUILD_TYPE_NONE TRUE)
endif()

