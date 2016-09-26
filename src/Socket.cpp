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
#include "SocketStatus.h"
#include "SocketCommand.h"
#include "SocketInterrupt.h"
#include "Platform.h"
#include <algorithm>

namespace eth
{

    Socket::Socket(SocketHandle handle) : m_handle(handle)
    {
    }

    bool Socket::open(Protocol protocol, uint16_t port, uint8_t flag)
    {
        if( protocol == Protocol::tcp )
        {
            close();
            device.writeSocketModeRegister(m_handle, static_cast<uint8_t>(protocol) | flag);

            // TODO: Check port for != 0
            device.writeSocketSourcePort(m_handle, port);
            device.executeSocketCommand(m_handle, SocketCommand::open);
            return true;
        }

        return false;
    }

    void Socket::close()
    {
        // TODO: Safe close in dtor
        device.executeSocketCommand(m_handle, SocketCommand::close);
        device.writeSocketInterruptRegister(m_handle, 0xff);
    }

    bool Socket::listen()
    {
        if( getStatus() != SocketStatus::init )
        {
            return false;
        }

        device.executeSocketCommand(m_handle, SocketCommand::listen);

        return true;
    }

    void Socket::accept()
    {
        while( getStatus() == SocketStatus::listen )
        {
            constexpr uint32_t waitTimeMs = 100;
            platform::stm32::wait(waitTimeMs);
        }
    }

    uint16_t Socket::send(const uint8_t* buffer, uint16_t length)
    {
        constexpr uint16_t bufferSize = device.getTransmitBufferSize();
        uint16_t sendSize = std::min(bufferSize, length);
        uint16_t freeSize = 0;

        do
        {
            freeSize = device.getTransmitFreeSize(m_handle);
            auto status = getStatus();

            if( (status != SocketStatus::established) && (status != SocketStatus::closeWait) )
            {
                return 0;
            }
        }
        while( freeSize < sendSize );

        device.sendData(m_handle, buffer, sendSize);
        device.executeSocketCommand(m_handle, SocketCommand::send);


        constexpr uint8_t send = static_cast<int>(SocketInterrupt::send);

        while( ( device.readSocketInterruptRegister(m_handle) & send ) != send )
        {
            if( getStatus() == SocketStatus::closed )
            {
                close();
                return 0;
            }
        }

        device.writeSocketInterruptRegister(m_handle, send);

        return sendSize;
    }

    SocketStatus Socket::getStatus() const
    {
        return device.readSocketStatusRegister(m_handle);
    }

}
