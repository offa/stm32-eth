
if( LTO )
    add_common_flag(-flto)
    add_linker_flag(-flto)


    find_program(CMAKE_AR_LTO ${CMAKE_C_COMPILER}-ar
                                NAMES gcc-ar
                                DOC "CMAKE_AR with LTO Plugin"
                                )

    if( CMAKE_AR_LTO )
        set(CMAKE_AR "${CMAKE_AR_LTO}")
        set(CMAKE_C_ARCHIVE_CREATE "<CMAKE_AR> qcs <TARGET> <LINK_FLAGS> <OBJECTS>")
        set(CMAKE_C_ARCHIVE_FINISH true)
        set(CMAKE_CXX_ARCHIVE_CREATE "<CMAKE_AR> qcs <TARGET> <LINK_FLAGS> <OBJECTS>")
        set(CMAKE_CXX_ARCHIVE_FINISH true)

        mark_as_advanced(CMAKE_AR_LTO)
    else()
        message(WARNING "CMAKE_AR_LTO not found")
    endif()

endif()

