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

#include <stdint.h>

namespace eth
{

    class SocketInterrupt
    {
    public:

        enum class Mask : uint8_t
        {
            connect = 0x01,
            disconnect = 0x02,
            receive = 0x04,
            timeout = 0x08,
            send = 0x10
        };


        constexpr explicit SocketInterrupt(uint8_t value) : m_value(value)
        {
        }

        constexpr SocketInterrupt(SocketInterrupt::Mask mask) : SocketInterrupt(static_cast<uint8_t>(mask))
        {
        }

        constexpr SocketInterrupt() : SocketInterrupt(0xff)
        {
        }


        constexpr uint8_t value() const
        {
            return m_value;
        }

        constexpr bool test(Mask mask) const
        {
            return m_value & static_cast<uint8_t>(mask);
        }

        constexpr void set(Mask mask)
        {
            m_value |= static_cast<uint8_t>(mask);
        }


    private:

        uint8_t m_value;
    };

}

