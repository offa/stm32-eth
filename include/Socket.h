/*
 * Stm32 Eth - Ethernet connectivity for Stm32
 * Copyright (C) 2016-2022  offa
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
#include "NetConfig.h"
#include <cstdint>
#include <span>

namespace eth
{
    namespace w5100
    {
        class Device;
    }


    class Socket
    {
    public:
        enum class Status : std::uint8_t
        {
            ok,
            failed,
            closed,
            timeout
        };


        Socket(SocketHandle socketHandle, w5100::Device& dev);
        Socket(const Socket&) = delete;
        ~Socket();


        Status open(Protocol protocol, std::uint16_t port, std::uint8_t flag);
        void close();
        Status listen();
        void accept();
        std::uint16_t send(const std::span<const std::uint8_t> buffer);
        std::uint16_t receive(std::span<std::uint8_t> buffer);


        Status connect(NetAddress<4> address, std::uint16_t port);
        Status disconnect();

        SocketStatus getStatus() const;


        Socket& operator=(const Socket&) = delete;


    private:
        bool isTimeouted() const;
        void closeImpl();


        SocketHandle handle;
        w5100::Device& device;
    };

}
