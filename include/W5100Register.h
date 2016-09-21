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

#pragma once

#include <stdint.h>

namespace eth
{

    class W5100Register
    {
    public:

        constexpr explicit W5100Register(uint16_t address) : m_address(address)
        {
        }


        constexpr uint16_t address() const
        {
            return m_address;
        }


    private:

        const uint16_t m_address;
    };


    constexpr W5100Register mode(0x0000);
    constexpr W5100Register transmitMemorySize(0x001b);
    constexpr W5100Register receiveMemorySize(0x001a);

    constexpr W5100Register gatewayAddress(0x0001);
    constexpr W5100Register subnetMask(0x0005);
    constexpr W5100Register sourceMacAddress(0x0009);
    constexpr W5100Register sourceIpAddress(0x000f);
}

