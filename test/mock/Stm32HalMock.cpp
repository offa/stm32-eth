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

#include "Platform.h"
#include <CppUTestExt/MockSupport.h>

HAL_StatusTypeDef HAL_SPI_Transmit(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    auto rtn = mock("HAL_SPI").actualCall("HAL_SPI_Transmit")
                .withPointerParameter("hspi", hspi)
                .withMemoryBufferParameter("pData", pData, Size)
                .withParameter("Size", Size)
                .withParameter("Timeout", Timeout)
                .returnUnsignedIntValueOrDefault(HAL_OK);
    return static_cast<HAL_StatusTypeDef>(rtn);
}

HAL_StatusTypeDef HAL_SPI_Receive(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    auto rtn = mock("HAL_SPI").actualCall("HAL_SPI_Receive")
                .withPointerParameter("hspi", hspi)
                .withOutputParameter("pData", pData)
                .withParameter("Size", Size)
                .withParameter("Timeout", Timeout)
                .returnUnsignedIntValueOrDefault(HAL_OK);
    return static_cast<HAL_StatusTypeDef>(rtn);

}

