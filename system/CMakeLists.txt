add_library(stm32hal-api INTERFACE)
target_compile_definitions(stm32hal-api
                            INTERFACE
                                STM32F407xx
                                USE_HAL_DRIVER
                                HSE_VALUE=8000000
                                OS_USE_TRACE_SEMIHOSTING_DEBUG
                            )
target_include_directories(stm32hal-api SYSTEM
                            INTERFACE
                                "${PROJECT_SOURCE_DIR}/system/include"
                                "${PROJECT_SOURCE_DIR}/system/include/stm32f4-hal"
                                "${PROJECT_SOURCE_DIR}/system/include/cmsis"
                            )


if( INTEGRATIONTEST OR BUILD_SYSTEM_DRIVER )
    add_subdirectory("src")
endif()

