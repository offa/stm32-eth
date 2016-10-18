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

    Socket::Socket(SocketHandle handle, W5100Device& device) : m_handle(handle), m_device(device)
    {
    }

    bool Socket::open(Protocol protocol, uint16_t port, uint8_t flag)
    {
        if( protocol == Protocol::tcp )
        {
            close();
            m_device.writeSocketModeRegister(m_handle, static_cast<uint8_t>(protocol) | flag);

            m_device.writeSocketSourcePort(m_handle, port);
            m_device.executeSocketCommand(m_handle, SocketCommand::open);
            return true;
        }

        return false;
    }

    void Socket::close()
    {
        // TODO: Safe close in dtor
        m_device.executeSocketCommand(m_handle, SocketCommand::close);
        m_device.writeSocketInterruptRegister(m_handle, 0xff);
    }

    bool Socket::listen()
    {
        if( getStatus() != SocketStatus::init )
        {
            return false;
        }

        m_device.executeSocketCommand(m_handle, SocketCommand::listen);

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

    uint16_t Socket::send(const gsl::span<const uint8_t> buffer)
    {
        if( buffer.empty() == true )
        {
            return 0;
        }

        const uint16_t sendSize = std::min(m_device.getTransmitBufferSize(), uint16_t(buffer.length()));
        const auto freeSize = waitForBuffer(sendSize);

        if( freeSize == 0 )
        {
            return 0;
        }

        m_device.sendData(m_handle, buffer);
        m_device.executeSocketCommand(m_handle, SocketCommand::send);


        constexpr uint8_t sendMask = static_cast<uint8_t>(SocketInterrupt::send);

        while( ( m_device.readSocketInterruptRegister(m_handle) & sendMask ) != sendMask )
        {
            if( getStatus() == SocketStatus::closed )
            {
                close();
                return 0;
            }
        }

        m_device.writeSocketInterruptRegister(m_handle, sendMask);

        return sendSize;
    }

    uint16_t Socket::receive(gsl::span<uint8_t> buffer)
    {
        const uint16_t available = waitForData();

        if( available == 0 )
        {
            return 0;
        }

        const uint16_t sizeLimited = std::min(m_device.getReceiveBufferSize(), uint16_t(buffer.length()));
        const uint16_t receiveSize = std::min(available, sizeLimited);
        auto shrinkedBuffer = buffer.first(receiveSize);
        m_device.receiveData(m_handle, shrinkedBuffer);
        m_device.executeSocketCommand(m_handle, SocketCommand::receive);

        return receiveSize;
    }

    SocketStatus Socket::getStatus() const
    {
        return m_device.readSocketStatusRegister(m_handle);
    }

    uint16_t Socket::waitForBuffer(uint16_t size) const
    {
        uint16_t freeSize = 0;

        do
        {
            freeSize = m_device.getTransmitFreeSize(m_handle);
            const SocketStatus status = getStatus();

            if( connectionReady(status) == false )
            {
                return 0;
            }
        }
        while( freeSize < size );

        return freeSize;
    }

    uint16_t Socket::waitForData() const
    {
        uint16_t available = 0;

        do
        {
            available = m_device.getReceiveFreeSize(m_handle);
            const SocketStatus status = getStatus();

            if( connectionReady(status) == false )
            {
                return 0;
            }
        }
        while( available == 0 );

        return available;
    }


}
