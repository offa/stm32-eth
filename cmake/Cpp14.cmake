
if( NOT CMAKE_CROSSCOMPILING )
    include(CheckCXXCompilerFlag)

    CHECK_CXX_COMPILER_FLAG("-std=c++14" CPP_STD_14)
    CHECK_CXX_COMPILER_FLAG("-std=c++1y" CPP_STD_1y)

    if( CPP_STD_14 )
        add_cxx_flag(-std=c++14)
    elseif( CPP_STD_0x )
        add_cxx_flag(-std=c++1y)
    else()
        message(WARNING "C++14 not supported by ${CMAKE_CXX_COMPILER}")
    endif()


    if( CMAKE_CXX_COMPILER_ID MATCHES "Clang" )
        if( $ENV{BROKEN_CLANG_WORKAROUND} )
            message(STATUS "Clang not using 'libc++'")
        else()
            add_cxx_flag(-stdlib=libc++)
        endif()
    endif()

else()
        add_cxx_flag(-std=c++14)
endif()

