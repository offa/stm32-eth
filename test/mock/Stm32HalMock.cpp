/*
 * Stm32 Eth - Ethernet connectivity for Stm32
 * Copyright (C) 2016 - 2017  offa
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

HAL_StatusTypeDef HAL_SPI_Init(SPI_HandleTypeDef *hspi)
{
    const auto rtn = mock("HAL_SPI").actualCall("HAL_SPI_Init")
                        .withPointerParameter("hspi", hspi)
                        .withPointerParameter("hspi.instance", hspi->Instance)
                        .withParameterOfType("SPI_InitTypeDef", "hspi.init", &hspi->Init)
                        .returnUnsignedIntValueOrDefault(HAL_OK);
    return static_cast<HAL_StatusTypeDef>(rtn);
}

HAL_StatusTypeDef HAL_SPI_Transmit(SPI_HandleTypeDef *hspi, std::uint8_t *pData, std::uint16_t Size, std::uint32_t Timeout)
{
    const auto rtn = mock("HAL_SPI").actualCall("HAL_SPI_Transmit")
                        .withPointerParameter("hspi", hspi)
                        .withMemoryBufferParameter("pData", pData, Size)
                        .withParameter("Size", Size)
                        .withParameter("Timeout", Timeout)
                        .returnUnsignedIntValueOrDefault(HAL_OK);
    return static_cast<HAL_StatusTypeDef>(rtn);
}

HAL_StatusTypeDef HAL_SPI_Receive(SPI_HandleTypeDef *hspi, std::uint8_t *pData, std::uint16_t Size, std::uint32_t Timeout)
{
    const auto rtn = mock("HAL_SPI").actualCall("HAL_SPI_Receive")
                        .withPointerParameter("hspi", hspi)
                        .withOutputParameter("pData", pData)
                        .withParameter("Size", Size)
                        .withParameter("Timeout", Timeout)
                        .returnUnsignedIntValueOrDefault(HAL_OK);
    return static_cast<HAL_StatusTypeDef>(rtn);
}

void HAL_GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_Init)
{
    mock("HAL_GPIO").actualCall("HAL_GPIO_Init")
        .withPointerParameter("GPIOx", GPIOx)
        .withParameterOfType("GPIO_InitTypeDef", "GPIO_Init", GPIO_Init);
}

void HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, std::uint16_t GPIO_Pin, GPIO_PinState PinState)
{
    mock("HAL_GPIO").actualCall("HAL_GPIO_WritePin")
        .withPointerParameter("GPIOx", GPIOx)
        .withParameter("GPIO_Pin", GPIO_Pin)
        .withParameter("PinState", PinState);
}

