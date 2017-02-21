set(CPP_STD 17)
set(CPP_STD_PRE 1z)

if( NOT CMAKE_CROSSCOMPILING )
    include(CheckCXXCompilerFlag)

    CHECK_CXX_COMPILER_FLAG("-std=c++${CPP_STD}" CPP_STD_SUPPORTED)
    CHECK_CXX_COMPILER_FLAG("-std=c++${CPP_STD_PRE}" CPP_STD_PRE_SUPPORTED)

    if( CPP_STD_SUPPORTED )
        add_cxx_flag(-std=c++${CPP_STD})
    elseif( CPP_STD_PRE_SUPPORTED )
        add_cxx_flag(-std=c++${CPP_STD_PRE})
    else()
        message(WARNING "C++${CPP_STD} not supported by ${CMAKE_CXX_COMPILER}")
    endif()

else()
    add_cxx_flag(-std=c++${CPP_STD})
endif()

