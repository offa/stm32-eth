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

#include "Socket.h"
#include "W5100Device.h"
#include "SocketMode.h"
#include "SocketStatus.h"
#include "SocketCommand.h"
#include "SocketInterrupt.h"

namespace eth
{
    void close(SocketHandle s)
    {
        device.executeSocketCommand(s, static_cast<uint8_t>(SocketCommand::close));
        device.writeSocketInterruptRegister(s, 0xff);
    }

    uint8_t socket(SocketHandle s, uint8_t protocol, uint16_t port, uint8_t flag)
    {
        if( protocol == static_cast<uint8_t>(SocketMode::tcp) )
        {
            close(s);
            device.writeSocketModeRegister(s, protocol | flag);

            // TODO: Check port for != 0
            device.writeSocketSourcePort(s, port);
            device.executeSocketCommand(s, static_cast<uint8_t>(SocketCommand::open));
            return 1;
        }

        return 0;
    }

    uint8_t listen(SocketHandle s)
    {
        if( device.readSocketStatusRegister(s) != static_cast<uint8_t>(SocketStatus::init) )
        {
            return 0;
        }

        device.executeSocketCommand(s, static_cast<uint8_t>(SocketCommand::listen));

        return 1;
    }


    uint16_t send(SocketHandle s, const uint8_t* buf, uint16_t len)
    {
        uint16_t ret = 0;
        uint16_t freeSize = 0;
        constexpr uint16_t bufferSize = device.getTransmitBufferSize();

        if( len > bufferSize )
        {
            ret = bufferSize;
        }
        else
        {
            ret = len;
        }

        do
        {
            freeSize = device.getTransmitFreeSize(s);
            SocketStatus status = static_cast<SocketStatus>(device.readSocketStatusRegister(s));

            if( (status != SocketStatus::established) && (status != SocketStatus::closeWait) )
            {
                // TODO: Return at this point?
                ret = 0;
                break;
            }
        }
        while( freeSize < ret );

        device.sendData(s, buf, ret);
        device.executeSocketCommand(s, static_cast<uint8_t>(SocketCommand::send));


        constexpr uint8_t sendOk = static_cast<int>(SocketInterrupt::sendOk);

        while( ( device.readSocketInterruptRegister(s) & sendOk ) != sendOk )
        {
            if( device.readSocketStatusRegister(s) == static_cast<uint8_t>(SocketStatus::closed) )
            {
                close(s);
                return 0;
            }
        }

        device.writeSocketInterruptRegister(s, sendOk);

        return ret;
    }

}
