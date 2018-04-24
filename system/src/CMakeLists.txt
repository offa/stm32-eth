add_library(cmsis cmsis/system_stm32f4xx.c
                    cmsis/vectors_stm32f407xx.c
                    )

add_library(cortexm cortexm/exception_handlers.c
                    cortexm/initialize-hardware.c
                    cortexm/reset-hardware.c
                    )

add_library(newlib newlib/assert.c
                    newlib/_cxx.cpp
                    newlib/_exit.c
                    newlib/_sbrk.c
                    newlib/_startup.c
                    newlib/_syscalls.c
                    )

add_library(hal stm32f4-hal/stm32f4xx_hal.c
                stm32f4-hal/stm32f4xx_hal_cortex.c
                stm32f4-hal/stm32f4xx_hal_flash.c
                stm32f4-hal/stm32f4xx_hal_gpio.c
                stm32f4-hal/stm32f4xx_hal_iwdg.c
                stm32f4-hal/stm32f4xx_hal_pwr.c
                stm32f4-hal/stm32f4xx_hal_rcc.c
                stm32f4-hal/stm32f4xx_hal_spi.c
                stm32f4-hal/stm32f4xx_hal_tim.c
                stm32f4-hal/stm32f4xx_hal_dma.c
                )


add_library(hw-init _initialize_hardware.c)
add_library(trace diag/Trace.c diag/trace_impl.c)

add_library(system-libs INTERFACE)

if( BUILD_TYPE_DEBUG )
    message(STATUS "HAL Debug enabled")
endif()


function(add_system_components)
    foreach( argTarget IN LISTS ARGN )
        if( BUILD_TYPE_DEBUG )
            target_compile_definitions(${argTarget} PUBLIC DEBUG USE_FULL_ASSERT TRACE)
        endif()

        target_link_libraries(${argTarget} PRIVATE stm32hal-api)
    endforeach()

    target_link_libraries(system-libs INTERFACE ${ARGN})
endfunction()

add_system_components(hal
                    cmsis
                    newlib
                    cortexm
                    hw-init
                    trace
                    )

