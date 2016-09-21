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

#include "SocketHandle.h"
#include <stdint.h>

namespace eth
{

    class W5100Register
    {
    public:

        constexpr W5100Register(uint16_t address, uint16_t size) : m_address(address), m_size(size)
        {
        }

        constexpr uint16_t address() const
        {
            return m_address;
        }

        constexpr uint16_t size() const
        {
            return m_size;
        }


    private:

        const uint16_t m_address;
        const uint16_t m_size;
    };


    template<class T>
    constexpr auto makeRegister(uint16_t address)
    {
        return W5100Register(address, sizeof(T));
    }


    constexpr W5100Register mode = makeRegister<uint8_t>(0x0000);
    constexpr W5100Register transmitMemorySize = makeRegister<uint8_t>(0x001b);
    constexpr W5100Register receiveMemorySize = makeRegister<uint8_t>(0x001a);

    constexpr W5100Register gatewayAddress = makeRegister<uint8_t>(0x0001);
    constexpr W5100Register subnetMask = makeRegister<uint8_t>(0x0005);
    constexpr W5100Register sourceMacAddress = makeRegister<uint8_t>(0x0009);
    constexpr W5100Register sourceIpAddress = makeRegister<uint8_t>(0x000f);



    template<class T>
    constexpr auto socketAddress(SocketHandle s, uint16_t address)
    {
        constexpr uint16_t baseAddress = 0x0400;
        constexpr uint16_t channelRegisterMapSize = 0x0100;
        return makeRegister<T>(baseAddress + s * channelRegisterMapSize + address);
    }


    constexpr W5100Register socketMode(SocketHandle s)
    {
        return socketAddress<uint8_t>(s, 0x0000);
    }

    constexpr W5100Register socketSourcePort(SocketHandle s)
    {
        return socketAddress<uint16_t>(s, 0x0004);
    }

    constexpr W5100Register socketInterrupt(SocketHandle s)
    {
        return socketAddress<uint8_t>(s, 0x0002);
    }

    constexpr W5100Register socketCommand(SocketHandle s)
    {
        return socketAddress<uint8_t>(s, 0x0001);
    }

    constexpr W5100Register socketStatus(SocketHandle s)
    {
        return socketAddress<uint8_t>(s, 0x0003);
    }

    constexpr W5100Register socketTransmitFreeSize(SocketHandle s)
    {
        return socketAddress<uint16_t>(s, 0x0020);
    }

    constexpr W5100Register socketTransmitWritePointer(SocketHandle s)
    {
        return socketAddress<uint16_t>(s, 0x0024);
    }

}

