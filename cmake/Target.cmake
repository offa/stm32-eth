
add_definitions(-DSTM32F407xx -DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -DOS_USE_TRACE_SEMIHOSTING_DEBUG)
include_directories(SYSTEM "system/include"
                            "system/include/stm32f4-hal"
                            "system/include/cmsis"
                            )


if( CMAKE_CROSSCOMPILING OR INTEGRATION_TEST )
    add_subdirectory("system")
endif()

