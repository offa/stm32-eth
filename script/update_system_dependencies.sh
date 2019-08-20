#!/bin/bash

SYSTEM_DIR="system"

cd ../${SYSTEM_DIR}

if [[ "${SYSTEM_DIR}" != "${PWD##*/}" ]]
then
    echo "Update failed - wrong path (${PWD})"
    exit 1
fi


load() {
    SUBDIR=${3:+/$3}

    if ! wget -nv "$1${SUBDIR}/$2" -O "$2"
    then
        echo "Unable to update $2"
        exit 1
    fi
}




pushd src/cmsis
URL_CMSIS_STM_SRC="https://raw.githubusercontent.com/xpacks/stm32f4-cmsis/xpack/Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates"
load ${URL_CMSIS_STM_SRC} system_stm32f4xx.c
#load ${URL_CMSIS_STM_SRC} vectors_stm32f407xx.c gcc    # Incompatible
popd


pushd src/cortexm
URL_CORTEXM_SRC="https://raw.githubusercontent.com/micro-os-plus/cortexm-startup/xpack/src/cortexm"
#load ${URL_CORTEXM_SRC} exception-handlers.c           # Incompatible
load ${URL_CORTEXM_SRC} initialize-hardware.c
load ${URL_CORTEXM_SRC} reset-hardware.c
popd


pushd src/stm32f4-hal
URL_STM_HAL_SRC="https://raw.githubusercontent.com/xpacks/stm32f4-hal/xpack/Drivers/STM32F4xx_HAL_Driver/Src"
load ${URL_STM_HAL_SRC} stm32f4xx_hal_adc.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_adc_ex.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_can.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_cec.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_cortex.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_crc.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_cryp.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_cryp_ex.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_dac.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_dac_ex.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_dcmi.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_dcmi_ex.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_dma2d.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_dma.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_dma_ex.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_dsi.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_eth.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_flash.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_flash_ex.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_flash_ramfunc.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_fmpi2c.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_fmpi2c_ex.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_gpio.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_hash.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_hash_ex.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_hcd.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_i2c.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_i2c_ex.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_i2s.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_i2s_ex.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_irda.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_iwdg.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_lptim.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_ltdc.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_ltdc_ex.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_msp_template.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_nand.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_nor.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_pccard.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_pcd.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_pcd_ex.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_pwr.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_pwr_ex.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_qspi.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_rcc.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_rcc_ex.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_rng.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_rtc.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_rtc_ex.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_sai.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_sai_ex.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_sd.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_sdram.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_smartcard.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_spdifrx.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_spi.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_sram.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_tim.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_timebase_tim_template.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_tim_ex.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_uart.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_usart.c
load ${URL_STM_HAL_SRC} stm32f4xx_hal_wwdg.c
load ${URL_STM_HAL_SRC} stm32f4xx_ll_fmc.c
load ${URL_STM_HAL_SRC} stm32f4xx_ll_fsmc.c
load ${URL_STM_HAL_SRC} stm32f4xx_ll_sdmmc.c
load ${URL_STM_HAL_SRC} stm32f4xx_ll_usb.c
popd


pushd include/cmsis
URL_CMSIS_ARM_INC="https://raw.githubusercontent.com/xpacks/arm-cmsis/xpack/CMSIS/Include"
URL_CMSIS_STM_INC="https://raw.githubusercontent.com/xpacks/stm32f4-cmsis/xpack/Drivers/CMSIS/Device/ST/STM32F4xx/Include/"

load ${URL_CMSIS_ARM_INC} arm_common_tables.h
load ${URL_CMSIS_ARM_INC} arm_const_structs.h
load ${URL_CMSIS_ARM_INC} arm_math.h
load ${URL_CMSIS_ARM_INC} cmsis_armcc.h
load ${URL_CMSIS_ARM_INC} cmsis_armcc_V6.h
load ${URL_CMSIS_ARM_INC} cmsis_gcc.h
load ${URL_CMSIS_ARM_INC} core_cm0.h
load ${URL_CMSIS_ARM_INC} core_cm0plus.h
load ${URL_CMSIS_ARM_INC} core_cm3.h
load ${URL_CMSIS_ARM_INC} core_cm4.h
load ${URL_CMSIS_ARM_INC} core_cm7.h
load ${URL_CMSIS_ARM_INC} core_cmFunc.h
load ${URL_CMSIS_ARM_INC} core_cmInstr.h
load ${URL_CMSIS_ARM_INC} core_cmSimd.h
load ${URL_CMSIS_ARM_INC} core_sc000.h
load ${URL_CMSIS_ARM_INC} core_sc300.h

load ${URL_CMSIS_STM_INC} cmsis_device.h
load ${URL_CMSIS_STM_INC} stm32f407xx.h
load ${URL_CMSIS_STM_INC} stm32f4xx.h
load ${URL_CMSIS_STM_INC} system_stm32f4xx.h
popd


pushd include/arm
URL_SEMIHOSTING_INC="https://raw.githubusercontent.com/micro-os-plus/micro-os-plus-iii/xpack/include/cmsis-plus/arm"
load ${URL_SEMIHOSTING_INC} semihosting.h
popd


#pushd include/cortexm
#URL_CORTEXM_INC="https://raw.githubusercontent.com/micro-os-plus/micro-os-plus-iii/xpack/include/cmsis-plus/cortexm"
#load ${URL_CORTEXM_INC} exception-handlers.h

#mv -f exception-handlers.h ExceptionHandlers.h     # Incompatible
#popd


pushd include/stm32f4-hal
URL_STM_HAL_INC="https://raw.githubusercontent.com/xpacks/stm32f4-hal/xpack/Drivers/STM32F4xx_HAL_Driver/Inc"
load ${URL_STM_HAL_INC} Legacy/stm32_hal_legacy.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_adc_ex.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_adc.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_can.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_cec.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_conf_template.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_cortex.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_crc.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_cryp_ex.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_cryp.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_dac_ex.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_dac.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_dcmi_ex.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_dcmi.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_def.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_dma2d.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_dma_ex.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_dma.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_dsi.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_eth.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_flash_ex.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_flash.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_flash_ramfunc.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_fmpi2c_ex.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_fmpi2c.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_gpio_ex.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_gpio.h
load ${URL_STM_HAL_INC} stm32f4xx_hal.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_hash_ex.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_hash.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_hcd.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_i2c_ex.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_i2c.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_i2s_ex.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_i2s.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_irda.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_iwdg.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_lptim.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_ltdc_ex.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_ltdc.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_nand.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_nor.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_pccard.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_pcd_ex.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_pcd.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_pwr_ex.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_pwr.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_qspi.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_rcc_ex.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_rcc.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_rng.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_rtc_ex.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_rtc.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_sai_ex.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_sai.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_sd.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_sdram.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_smartcard.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_spdifrx.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_spi.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_sram.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_tim_ex.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_tim.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_uart.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_usart.h
load ${URL_STM_HAL_INC} stm32f4xx_hal_wwdg.h
load ${URL_STM_HAL_INC} stm32f4xx_ll_fmc.h
load ${URL_STM_HAL_INC} stm32f4xx_ll_fsmc.h
load ${URL_STM_HAL_INC} stm32f4xx_ll_sdmmc.h
load ${URL_STM_HAL_INC} stm32f4xx_ll_usb.h
popd

