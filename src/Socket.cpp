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

    Socket::Socket(SocketHandle handle) : m_handle(handle)
    {
    }

    bool Socket::open(uint8_t protocol, uint16_t port, uint8_t flag)
    {
        if( protocol == static_cast<uint8_t>(SocketMode::tcp) )
        {
            close();
            device.writeSocketModeRegister(m_handle, protocol | flag);

            // TODO: Check port for != 0
            device.writeSocketSourcePort(m_handle, port);
            device.executeSocketCommand(m_handle, static_cast<uint8_t>(SocketCommand::open));
            return 1;
        }

        return 0;
    }

    void Socket::close()
    {
        // TODO: Safe close in dtor
        device.executeSocketCommand(m_handle, static_cast<uint8_t>(SocketCommand::close));
        device.writeSocketInterruptRegister(m_handle, 0xff);
    }

    bool Socket::listen()
    {
        if( device.readSocketStatusRegister(m_handle) != static_cast<uint8_t>(SocketStatus::init) )
        {
            return false;
        }

        device.executeSocketCommand(m_handle, static_cast<uint8_t>(SocketCommand::listen));

        return true;
    }

    uint16_t Socket::send(const uint8_t* buf, uint16_t len)
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
            freeSize = device.getTransmitFreeSize(m_handle);
            SocketStatus status = static_cast<SocketStatus>(device.readSocketStatusRegister(m_handle));

            if( (status != SocketStatus::established) && (status != SocketStatus::closeWait) )
            {
                // TODO: Return at this point?
                ret = 0;
                break;
            }
        }
        while( freeSize < ret );

        device.sendData(m_handle, buf, ret);
        device.executeSocketCommand(m_handle, static_cast<uint8_t>(SocketCommand::send));


        constexpr uint8_t sendOk = static_cast<int>(SocketInterrupt::sendOk);

        while( ( device.readSocketInterruptRegister(m_handle) & sendOk ) != sendOk )
        {
            if( device.readSocketStatusRegister(m_handle) == static_cast<uint8_t>(SocketStatus::closed) )
            {
                close();
                return 0;
            }
        }

        device.writeSocketInterruptRegister(m_handle, sendOk);

        return ret;
    }

}
