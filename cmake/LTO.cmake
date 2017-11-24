
if( LTO )
    if( NOT CMAKE_CROSSCOMPILING )
        include(CheckIPOSupported)
        check_ipo_supported(RESULT lto_supported OUTPUT lto_error)
    else()
        set(lto_supported TRUE)
    endif()


    if( lto_supported )
        message(STATUS "IPO / LTO enabled")
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    else()
        message(STATUS "IPO / LTO not supported:\n-------\n${error}\n-------")
    endif()
endif()

