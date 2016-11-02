/*
 * Stm32 Eth - Ethernet connectivity for Stm32
 * Copyright (C) 2016  offa
 *
 * This file is part of Stm32 Eth.
 *
 * Stm32 Eth is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Stm32 Eth is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Stm32 Eth.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "SpiWriter.h"
#include "Byte.h"
#include <array>

namespace eth
{

    SpiWriter::SpiWriter()
    {
        init();
    }


    void SpiWriter::init()
    {
        platform::spiClockEnable();

        GPIO_InitTypeDef gpioSpi{(GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15),
                                GPIO_MODE_AF_PP, GPIO_NOPULL,
                                GPIO_SPEED_HIGH, GPIO_AF5_SPI2};
        HAL_GPIO_Init(GPIOB, &gpioSpi);

        GPIO_InitTypeDef gpioSS{GPIO_PIN_12, GPIO_MODE_OUTPUT_PP,
                                GPIO_PULLUP, GPIO_SPEED_LOW,
                                GPIO_AF5_SPI2};
        HAL_GPIO_Init(GPIOB, &gpioSS);


        constexpr SPI_InitTypeDef settings{SPI_MODE_MASTER,
                                            SPI_DIRECTION_2LINES,
                                            SPI_DATASIZE_8BIT,
                                            SPI_POLARITY_LOW,
                                            SPI_PHASE_1EDGE,
                                            SPI_NSS_SOFT,
                                            SPI_BAUDRATEPRESCALER_4,
                                            SPI_FIRSTBIT_MSB,
                                            SPI_TIMODE_DISABLED,
                                            SPI_CRCCALCULATION_DISABLED,
                                            0};
        m_handle.Instance = SPI2;
        m_handle.Init = settings;

        HAL_SPI_Init(&m_handle);
    }

    void SpiWriter::write(uint16_t address, uint8_t data)
    {
        std::array<uint8_t, 4> a = {{ 0xf0,
            byte::get<1>(address),
            byte::get<0>(address),
            data
        }};

        setSlaveSelect();
        HAL_SPI_Transmit(&m_handle, a.data(), a.size(), timeout);
        resetSlaveSelect();
    }

    uint8_t SpiWriter::read(uint16_t address)
    {
        std::array<uint8_t, 3> a = {{ 0x0f,
            byte::get<1>(address),
            byte::get<0>(address)
        }};

        setSlaveSelect();
        HAL_SPI_Transmit(&m_handle, a.data(), a.size(), timeout);

        uint8_t buffer;
        HAL_SPI_Receive(&m_handle, &buffer, sizeof(buffer), timeout);
        resetSlaveSelect();

        return buffer;
    }

    void SpiWriter::setSlaveSelect()
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
    }

    void SpiWriter::resetSlaveSelect()
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
    }

    SpiWriter::Handle& SpiWriter::nativeHandle()
    {
        return m_handle;
    }

}

