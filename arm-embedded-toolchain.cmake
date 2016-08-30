include(CMakeForceCompiler)

set(CMAKE_SYSTEM_NAME Generic)

CMAKE_FORCE_C_COMPILER(arm-none-eabi-gcc GNU)
CMAKE_FORCE_CXX_COMPILER(arm-none-eabi-g++ GNU)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

macro(add_common_flag)
    foreach(_flag ${ARGN})
        set(C_FLAGS "${C_FLAGS} ${_flag}")
        set(CXX_FLAGS "${CXX_FLAGS} ${_flag}")
    endforeach()
endmacro()

macro(add_cxx_flag)
    foreach(_flag ${ARGN})
        set(CXX_FLAGS "${CXX_FLAGS} ${_flag}")
    endforeach()
endmacro()

add_common_flag(-mcpu=cortex-m4
                -mthumb
                -mfloat-abi=soft
                -ffunction-sections
                -fdata-sections
                -ffreestanding
                )

add_cxx_flag(-fno-exceptions
                -fno-rtti
                -fno-use-cxa-atexit
                -fno-threadsafe-statics
                )

set(CMAKE_C_FLAGS ${C_FLAGS} CACHE STRING "C Compiler Flags")
set(CMAKE_CXX_FLAGS ${CXX_FLAGS} CACHE STRING "C++ Compiler Flags")


