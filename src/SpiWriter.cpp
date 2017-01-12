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

#include "SpiWriter.h"
#include "Byte.h"
#include <array>

namespace eth
{
    namespace
    {
        enum class OpCode : uint8_t
        {
            read = 0x0f,
            write = 0xf0
        };


        template<OpCode opcode, class... Ts>
        constexpr std::array<uint8_t, 3 + sizeof...(Ts)> makePacket(uint16_t address, Ts&&... params)
        {
            return {{ static_cast<uint8_t>(opcode),
                    byte::get<1>(address),
                    byte::get<0>(address),
                    params... }};
        }


        const std::array<SPI_TypeDef*, 3> spiInstances{{SPI1, SPI2, SPI3}};

    }



    SpiWriter::SpiWriter(const SpiConfig& config)
    {
        Assign spi;
        GPIO_InitTypeDef gpio;
        GPIO_InitTypeDef gpioSS;
        SPI_InitTypeDef settings;
        std::tie(spi, gpio, gpioSS, settings) = config;

        HAL_GPIO_Init(GPIOB, &gpio);
        HAL_GPIO_Init(GPIOB, &gpioSS);

        m_handle.Instance = spiInstances[static_cast<size_t>(spi)];
        m_handle.Init = settings;

        HAL_SPI_Init(&m_handle);
    }

    void SpiWriter::write(uint16_t address, uint8_t data)
    {
        auto packet = makePacket<OpCode::write>(address, data);

        setSlaveSelect();
        HAL_SPI_Transmit(&m_handle, packet.data(), packet.size(), timeout);
        resetSlaveSelect();
    }

    uint8_t SpiWriter::read(uint16_t address)
    {
        auto packet = makePacket<OpCode::read>(address);

        setSlaveSelect();
        HAL_SPI_Transmit(&m_handle, packet.data(), packet.size(), timeout);

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

