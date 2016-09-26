
#include "Platform.h"

namespace platform
{
    namespace stm32
    {
        void spiClockEnable()
        {
            __HAL_RCC_GPIOB_CLK_ENABLE();
            __HAL_RCC_SPI2_CLK_ENABLE();
        }

        void wait(uint32_t timeMs)
        {
            HAL_Delay(timeMs);
        }
    }
}

