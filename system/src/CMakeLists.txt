add_library(cmsis OBJECT cmsis/system_stm32f4xx.c cmsis/vectors_stm32f407xx.c)

add_library(cortexm OBJECT cortexm/exception_handlers.c cortexm/_initialize_hardware.c cortexm/_reset_hardware.c)

add_library(newlib OBJECT newlib/assert.c newlib/_cxx.cpp newlib/_exit.c newlib/_sbrk.c newlib/_startup.c newlib/_syscalls.c)

add_library(hal OBJECT stm32f4-hal/stm32f4xx_hal.c
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

add_library(misc OBJECT _initialize_hardware.c stm32f4xx_hal_msp.c)

add_library(trace OBJECT diag/Trace.c diag/trace_impl.c)


set_target_properties(cmsis cortexm newlib hal misc trace PROPERTIES COMPILE_FLAGS "-Wno-error -Wno-pedantic -Wno-extra")

