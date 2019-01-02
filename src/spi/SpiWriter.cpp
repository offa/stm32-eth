/*
 * Stm32 Eth - Ethernet connectivity for Stm32
 * Copyright (C) 2016-2019  offa
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

#include "spi/SpiWriter.h"
#include "Byte.h"
#include <array>

namespace eth::spi
{
    namespace
    {
        enum class OpCode : std::uint8_t
        {
            read = 0x0f,
            write = 0xf0
        };


        template<OpCode opcode, class... Ts>
        constexpr auto makePacket(std::uint16_t address, Ts&&... params)
        {
            using Type = std::array<std::uint8_t, 3 + sizeof...(Ts)>;
            return Type{{ static_cast<std::uint8_t>(opcode),
                            byte::get<1>(address),
                            byte::get<0>(address),
                            params... }};
        }

        constexpr auto timeout = std::numeric_limits<std::uint32_t>::max();

        const std::array<SPI_TypeDef*, 3> spiInstances{{SPI1, SPI2, SPI3}};
        const std::array<GPIO_TypeDef*, 3> pinBlocks{{GPIOA, GPIOB, GPIOC}};
    }


    class SpiWriter::SlaveSelect
    {
    public:

        explicit SlaveSelect(SpiWriter* writer) : m_writer(writer)
        {
            m_writer->setSlaveSelect(PinState::set);
        }

        SlaveSelect(SlaveSelect&&) = delete;
        SlaveSelect(const SlaveSelect&) = delete;

        ~SlaveSelect()
        {
            m_writer->setSlaveSelect(PinState::reset);
        }


        SlaveSelect& operator=(SlaveSelect&&) = delete;
        SlaveSelect& operator=(const SlaveSelect&) = delete;


    private:

        SpiWriter* m_writer;
    };


    SpiWriter::SpiWriter(const SpiConfig& config) : m_config(config)
    {
        auto[spi, block, gpio, gpioSS, settings] = m_config;
        const auto blockRef = pinBlocks[static_cast<std::size_t>(block)];

        HAL_GPIO_Init(blockRef, &gpio);
        HAL_GPIO_Init(blockRef, &gpioSS);

        m_handle.Instance = spiInstances[static_cast<std::size_t>(spi)];
        m_handle.Init = settings;

        HAL_SPI_Init(&m_handle);
    }

    void SpiWriter::write(std::uint16_t address, std::uint8_t data)
    {
        auto packet = makePacket<OpCode::write>(address, data);

        SlaveSelect ss{this};
        HAL_SPI_Transmit(&m_handle, packet.data(), packet.size(), timeout);
    }

    std::uint8_t SpiWriter::read(std::uint16_t address)
    {
        auto packet = makePacket<OpCode::read>(address);

        SlaveSelect ss{this};
        HAL_SPI_Transmit(&m_handle, packet.data(), packet.size(), timeout);

        std::array<std::uint8_t, 1> buffer{{0}};
        HAL_SPI_Receive(&m_handle, buffer.data(), buffer.size(), timeout);

        return buffer[0];
    }

    void SpiWriter::setSlaveSelect(PinState state)
    {
        const auto value = ( state == PinState::set ? GPIO_PIN_RESET : GPIO_PIN_SET );
        const auto blockRef = pinBlocks[static_cast<std::size_t>(std::get<1>(m_config))];
        HAL_GPIO_WritePin(blockRef, std::get<3>(m_config).Pin, value);
    }

    SpiWriter::Handle& SpiWriter::nativeHandle() noexcept
    {
        return m_handle;
    }

}
