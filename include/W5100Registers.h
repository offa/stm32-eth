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

#include "W5100Register.h"

namespace eth
{

    namespace w5100
    {

        constexpr W5100Register mode = makeRegister<uint8_t>(0x0000);
        constexpr W5100Register transmitMemorySize = makeRegister<uint8_t>(0x001b);
        constexpr W5100Register receiveMemorySize = makeRegister<uint8_t>(0x001a);

        constexpr W5100Register gatewayAddress = makeRegister<uint8_t>(0x0001);
        constexpr W5100Register subnetMask = makeRegister<uint8_t>(0x0005);
        constexpr W5100Register sourceMacAddress = makeRegister<uint8_t>(0x0009);
        constexpr W5100Register sourceIpAddress = makeRegister<uint8_t>(0x000f);



        constexpr W5100Register socketMode(SocketHandle s)
        {
            return makeSocketRegister<uint8_t>(s, 0x0000);
        }

        constexpr W5100Register socketSourcePort(SocketHandle s)
        {
            return makeSocketRegister<uint16_t>(s, 0x0004);
        }

        constexpr W5100Register socketInterrupt(SocketHandle s)
        {
            return makeSocketRegister<uint8_t>(s, 0x0002);
        }

        constexpr W5100Register socketCommand(SocketHandle s)
        {
            return makeSocketRegister<uint8_t>(s, 0x0001);
        }

        constexpr W5100Register socketStatus(SocketHandle s)
        {
            return makeSocketRegister<uint8_t>(s, 0x0003);
        }

        constexpr W5100Register socketTransmitFreeSize(SocketHandle s)
        {
            return makeSocketRegister<uint16_t>(s, 0x0020);
        }

        constexpr W5100Register socketReceiveFreeSize(SocketHandle s)
        {
            return makeSocketRegister<uint16_t>(s, 0x0026);
        }

        constexpr W5100Register socketTransmitWritePointer(SocketHandle s)
        {
            return makeSocketRegister<uint16_t>(s, 0x0024);
        }

        constexpr W5100Register socketReceiveReadPointer(SocketHandle s)
        {
            return makeSocketRegister<uint16_t>(s, 0x0028);
        }

        constexpr W5100Register socketDestIpAddress(SocketHandle s)
        {
            return makeSocketRegister<uint8_t>(s, 0x000c);
        }

        constexpr W5100Register socketDestPort(SocketHandle s)
        {
            return makeSocketRegister<uint8_t>(s, 0x0010);
        }

    }

}
