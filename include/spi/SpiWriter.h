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

#pragma once

#include "spi/SpiConfig.h"

namespace eth::spi
{

    class SpiWriter
    {
    public:

        using Handle = SPI_HandleTypeDef;


        SpiWriter(const SpiConfig& config);
        SpiWriter(SpiWriter&&) = default;


        void write(std::uint16_t address, std::uint8_t data);
        std::uint8_t read(std::uint16_t address);

        Handle& nativeHandle();


        SpiWriter& operator=(SpiWriter&&) = default;


    private:

        enum class PinState : std::uint8_t
        {
            set,
            reset
        };

        void setSlaveSelect(PinState state);

        class SlaveSelect;

        Handle m_handle{};
        const SpiConfig& m_config;
        static constexpr std::uint32_t timeout = ~0;
    };

}

