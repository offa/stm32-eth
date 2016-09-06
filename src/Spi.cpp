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

