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

#include "Spi.h"

namespace eth
{

    void Spi::init()
    {
        platform::stm32::spiClockEnable();

        GPIO_InitTypeDef gpioInit;
        gpioInit.Pin = GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
        gpioInit.Mode = GPIO_MODE_AF_PP;
        gpioInit.Pull = GPIO_NOPULL;
        gpioInit.Speed = GPIO_SPEED_HIGH;
        gpioInit.Alternate = GPIO_AF5_SPI2;
        HAL_GPIO_Init(GPIOB, &gpioInit);

        GPIO_InitTypeDef gpioInitSS;
        gpioInitSS.Pin = GPIO_PIN_12;
        gpioInitSS.Mode = GPIO_MODE_OUTPUT_PP;
        gpioInitSS.Pull = GPIO_PULLUP;
        gpioInitSS.Speed = GPIO_SPEED_LOW;
        gpioInitSS.Alternate = GPIO_AF5_SPI2;
        HAL_GPIO_Init(GPIOB, &gpioInitSS);

        m_handle.Instance = SPI2;
        m_handle.Init.Mode = SPI_MODE_MASTER;
        m_handle.Init.Direction = SPI_DIRECTION_2LINES;
        m_handle.Init.DataSize = SPI_DATASIZE_8BIT;
        m_handle.Init.CLKPolarity = SPI_POLARITY_LOW;
        m_handle.Init.CLKPhase = SPI_PHASE_1EDGE;
        m_handle.Init.NSS = SPI_NSS_SOFT;
        m_handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
        m_handle.Init.FirstBit = SPI_FIRSTBIT_MSB;
        m_handle.Init.TIMode = SPI_TIMODE_DISABLED;
        m_handle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
        m_handle.Init.CRCPolynomial = 0;

        HAL_SPI_Init(&m_handle);
    }

    void Spi::transmit(uint8_t data)
    {
        // TODO: Check returnvalue
        HAL_SPI_Transmit(&m_handle, &data, sizeof(data), timeout);
    }

    uint8_t Spi::receive()
    {
        // TODO: Check returnvalue
        uint8_t buffer;
        HAL_SPI_Receive(&m_handle, &buffer, sizeof(buffer), timeout);
        return buffer;
    }

    void Spi::setSlaveSelect()
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
    }

    void Spi::resetSlaveSelect()
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
    }

    Spi::Handle& Spi::nativeHandle()
    {
        return m_handle;
    }

}

