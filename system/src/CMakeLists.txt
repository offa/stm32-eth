add_c_library(cmsis cmsis/system_stm32f4xx.c
                    cmsis/vectors_stm32f407xx.c
                    )

add_c_library(cortexm cortexm/exception_handlers.c
                    cortexm/initialize-hardware.c
                    cortexm/reset-hardware.c
                    )

add_c_library(newlib newlib/assert.c
                    newlib/_cxx.cpp
                    newlib/_exit.c
                    newlib/_sbrk.c
                    newlib/_startup.c
                    newlib/_syscalls.c
                    )

add_c_library(hal stm32f4-hal/stm32f4xx_hal.c
                stm32f4-hal/stm32f4xx_hal_cortex.c
                stm32f4-hal/stm32f4xx_hal_flash.c
                stm32f4-hal/stm32f4xx_hal_gpio.c
                stm32f4-hal/stm32f4xx_hal_iwdg.c
                stm32f4-hal/stm32f4xx_hal_pwr.c
                stm32f4-hal/stm32f4xx_hal_rcc.c
                stm32f4-hal/stm32f4xx_hal_spi.c
                stm32f4-hal/stm32f4xx_hal_tim.c
                stm32f4-hal/stm32f4xx_hal_dma.c
                stm32f4-hal/stm32f4xx_hal_msp_template.c
                )


add_c_library(hw-init _initialize_hardware.c)
add_c_library(trace diag/Trace.c diag/trace_impl.c)


add_library(system-libs INTERFACE)
target_link_libraries(system-libs
                        INTERFACE
                            hal
                            cmsis
                            newlib
                            cortexm
                            hw-init
                            trace
                            )


function(disable_compiler_warnings target_name)
    target_compile_options(${target_name} PUBLIC
                                -Wno-error
                                -Wno-pedantic
                                -Wno-extra
                                )
endfunction()

disable_compiler_warnings(cmsis)
disable_compiler_warnings(cortexm)
disable_compiler_warnings(newlib)
disable_compiler_warnings(hal)
disable_compiler_warnings(hw-init)
disable_compiler_warnings(trace)
