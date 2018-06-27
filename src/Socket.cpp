/*
 * Stm32 Eth - Ethernet connectivity for Stm32
 * Copyright (C) 2016-2018  offa
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
#include "w5100/Device.h"
#include "SocketStatus.h"
#include "SocketCommand.h"
#include "Platform.h"
#include <algorithm>

namespace eth
{
    namespace
    {

        constexpr bool connectionReady(SocketStatus status)
        {
            return ( status == SocketStatus::established )
                    || ( status == SocketStatus::closeWait );
        }

        template<class DataFn, class StatusFn>
        std::uint16_t waitFor(DataFn getDataFn, StatusFn statusCheckFn, std::uint16_t size)
        {
            while( statusCheckFn() == true )
            {
                const auto actual = getDataFn();

                if( actual >= size )
                {
                    return actual;
                }
            }

            return 0;
        }

    }



    Socket::Socket(SocketHandle handle, w5100::Device& device) : m_handle(handle), m_device(device)
    {
    }

    Socket::~Socket()
    {
        closeImpl();
    }

    Socket::Status Socket::open(Protocol protocol, std::uint16_t port, std::uint8_t flag)
    {
        if( protocol == Protocol::tcp )
        {
            close();
            m_device.writeSocketModeRegister(m_handle, static_cast<std::uint8_t>(protocol) | flag);

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

    std::uint16_t Socket::send(const gsl::span<const std::uint8_t> buffer)
    {
        if( buffer.empty() == true )
        {
            return 0;
        }

        const std::uint16_t sendSize = std::min<std::uint16_t>(w5100::Device::getTransmitBufferSize(), buffer.size());
        const auto freeSize = waitFor([this] { return m_device.getTransmitFreeSize(m_handle); },
                                        [this] { return connectionReady(getStatus()); },
                                        sendSize);

        if( freeSize == 0 )
        {
            return 0;
        }

        m_device.sendData(m_handle, buffer);
        m_device.executeSocketCommand(m_handle, SocketCommand::send);

        return sendSize;
    }

    std::uint16_t Socket::receive(gsl::span<std::uint8_t> buffer)
    {
        if( buffer.empty() == true )
        {
            return 0;
        }

        const std::uint16_t available = waitFor([this] { return m_device.getReceiveFreeSize(m_handle); },
                                            [this] { return connectionReady(getStatus()); },
                                            1);


        if( available == 0 )
        {
            return 0;
        }

        const std::uint16_t sizeLimited = std::min<std::uint16_t>(w5100::Device::getReceiveBufferSize(), buffer.size());
        const std::uint16_t receiveSize = std::min(available, sizeLimited);
        auto shrinkedBuffer = buffer.first(receiveSize);
        m_device.receiveData(m_handle, shrinkedBuffer);
        m_device.executeSocketCommand(m_handle, SocketCommand::receive);

        return receiveSize;
    }

    Socket::Status Socket::connect(NetAddress<4> address, std::uint16_t port)
    {
        m_device.setDestAddress(m_handle, address, port);
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
