
include(CheckCXXCompilerFlag)

CHECK_CXX_COMPILER_FLAG("-std=c++14" CPP_STD_14)
CHECK_CXX_COMPILER_FLAG("-std=c++1y" CPP_STD_1y)

if( CPP_STD_14 )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
elseif( CPP_STD_0x )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")
else()
    message(WARNING "C++14 not supported by ${CMAKE_CXX_COMPILER}")
endif()
