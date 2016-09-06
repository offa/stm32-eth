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
#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>

TEST_GROUP(SpiTest)
{
    void setup() override
    {
        spi = std::make_unique<eth::Spi>();
    }

    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
    }

    std::unique_ptr<eth::Spi> spi;
    MockSupport& halSpi = mock("HAL_SPI");
    MockSupport& halGpio = mock("HAL_GPIO");
    static constexpr uint32_t timeout = 0xffffffff;
};

TEST(SpiTest, transmitTransmitsByte)
{
    const uint8_t data = 0xab;
    constexpr uint16_t size = sizeof(data);

    halSpi.expectOneCall("HAL_SPI_Transmit")
        .withPointerParameter("hspi", &spi->nativeHandle())
        .withMemoryBufferParameter("pData", &data, size)
        .withParameter("Size", size)
        .withParameter("Timeout", timeout);

    spi->transmit(data);
}

TEST(SpiTest, receiveReceivesByte)
{
    const uint8_t data = 0xcd;
    constexpr uint16_t size = sizeof(data);

    halSpi.expectOneCall("HAL_SPI_Receive")
        .withPointerParameter("hspi", &spi->nativeHandle())
        .withOutputParameterReturning("pData", &data, size)
        .withParameter("Size", size)
        .withParameter("Timeout", timeout);

    auto result = spi->receive();
    CHECK_EQUAL(data, result);
}

TEST(SpiTest, setSlaveSelectSetsPinLow)
{
    halGpio.expectOneCall("HAL_GPIO_WritePin")
        .withPointerParameter("GPIOx", GPIOB)
        .withParameter("GPIO_Pin", GPIO_PIN_12)
        .withParameter("PinState", GPIO_PIN_RESET);

    spi->setSlaveSelect();
}

TEST(SpiTest, resetSlaveSelectSetsPinHigh)
{
    halGpio.expectOneCall("HAL_GPIO_WritePin")
        .withPointerParameter("GPIOx", GPIOB)
        .withParameter("GPIO_Pin", GPIO_PIN_12)
        .withParameter("PinState", GPIO_PIN_SET);

    spi->resetSlaveSelect();
}

