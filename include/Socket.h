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
#include "SocketStatus.h"
#include "SocketInterrupt.h"
#include "Protocol.h"
#include <array>
#include <stdint.h>
#include <gsl/span>

namespace eth
{
    class W5100Device;


    class Socket
    {
    public:

        enum class Status : uint8_t
        {
            ok,
            closed,
            timeout
        };


        Socket(SocketHandle handle, W5100Device& device);
        Socket(Socket&&) = default;


        bool open(Protocol protocol, uint16_t port, uint8_t flag);
        void close();
        bool listen();
        void accept();
        uint16_t send(const gsl::span<const uint8_t> buffer);
        uint16_t receive(gsl::span<uint8_t> buffer);



        Status connect(std::array<uint8_t,4> address, uint16_t port);
        Status disconnect();

        SocketStatus getStatus() const;


        Socket& operator=(Socket&&) = default;


    private:

        uint16_t waitForBuffer(uint16_t size) const;
        uint16_t waitForData() const;
        bool isTimeouted() const;

        static constexpr bool connectionReady(SocketStatus status)
        {
            return ( status == SocketStatus::established )
                    || ( status == SocketStatus::closeWait );
        }


        const SocketHandle m_handle;
        W5100Device& m_device;
    };

}

