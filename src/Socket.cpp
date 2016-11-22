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
#include "Platform.h"
#include <algorithm>

namespace eth
{

    constexpr bool connectionReady(SocketStatus status)
    {
        return ( status == SocketStatus::established )
                || ( status == SocketStatus::closeWait );
    }


    Socket::Socket(SocketHandle handle, W5100Device& device) : m_handle(handle), m_device(device)
    {
    }

    Socket::~Socket() {
        closeImpl();
    }

    Socket::Status Socket::open(Protocol protocol, uint16_t port, uint8_t flag)
    {
        if( protocol == Protocol::tcp )
        {
            close();
            m_device.writeSocketModeRegister(m_handle, static_cast<uint8_t>(protocol) | flag);

            m_device.writeSocketSourcePort(m_handle, port);
            m_device.executeSocketCommand(m_handle, SocketCommand::open);

            while( getStatus() == SocketStatus::closed )
            {
                // Wait for completion
            }

            return Status::ok;
        }

        return Status::failed;
    }

    void Socket::close()
    {
        closeImpl();

        while( getStatus() != SocketStatus::closed )
        {
            // Wait for completion
        }
    }

    Socket::Status Socket::listen()
    {
        if( getStatus() != SocketStatus::init )
        {
            return Status::closed;
        }

        m_device.executeSocketCommand(m_handle, SocketCommand::listen);

        return Status::ok;
    }

    void Socket::accept()
    {
        while( getStatus() == SocketStatus::listen )
        {
            using namespace std::chrono_literals;
            platform::wait(100ms);
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

        return sendSize;
    }

    uint16_t Socket::receive(gsl::span<uint8_t> buffer)
    {
        if( buffer.empty() == true )
        {
            return 0;
        }

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

    Socket::Status Socket::connect(std::array<uint8_t,4> address, uint16_t port)
    {
        m_device.setDestIpAddress(m_handle, address);
        m_device.setDestPort(m_handle, port);
        m_device.executeSocketCommand(m_handle, SocketCommand::connect);

        while( getStatus() != SocketStatus::established )
        {
            if( getStatus() == SocketStatus::closed )
            {
                return Status::closed;
            }

            if( isTimeouted() == true )
            {
                return Status::timeout;
            }
        }

        return Status::ok;
    }

    Socket::Status Socket::disconnect()
    {
        m_device.executeSocketCommand(m_handle, SocketCommand::disconnect);

        while( getStatus() != SocketStatus::closed )
        {
            if( isTimeouted() == true )
            {
                return Status::timeout;
            }
        }

        return Status::ok;
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

    bool Socket::isTimeouted() const
    {
        const auto value = m_device.readSocketInterruptRegister(m_handle);
        return value.test(SocketInterrupt::Mask::timeout);
    }

    void Socket::closeImpl()
    {
        m_device.executeSocketCommand(m_handle, SocketCommand::close);
        m_device.writeSocketInterruptRegister(m_handle, SocketInterrupt(0xff));
    }

}
