
macro(enable_build_type)
    list(APPEND VALID_BUILD_TYPES ${ARGN})
endmacro()

macro(set_build_type type)
    list(FIND VALID_BUILD_TYPES ${type} type_found)

    if( NOT ( ${type_found} LESS 0 ) )
        string(TOUPPER ${type} type_name)
        set(BUILD_TYPE_${type_name} TRUE)
    else()
        message(SEND_ERROR "Warning:\tInvalid / unsupported build type: '${CMAKE_BUILD_TYPE}'")
    endif()
macro()


enable_build_type(Debug
                Release
                RelWithDebInfo
                MinSizeRel
                )


if( CMAKE_BUILD_TYPE )
    set_build_type(${CMAKE_BUILD_TYPE})
else()
    set(BUILD_TYPE_NONE TRUE)
endif()

