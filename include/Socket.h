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
#include "Protocol.h"
#include <stdint.h>

namespace eth
{

    class Socket
    {
    public:

        explicit Socket(SocketHandle handle);
        Socket(Socket&&) = default;


        bool open(Protocol protocol, uint16_t port, uint8_t flag);
        void close();
        bool listen();
        void accept();
        uint16_t send(const uint8_t* buffer, uint16_t length);
        uint16_t receive(uint8_t* buffer, uint16_t length);
        SocketStatus getStatus() const;


        Socket& operator=(Socket&&) = default;


    private:

        uint16_t waitForData() const;

        static constexpr bool connectionReady(SocketStatus status)
        {
            return ( status == SocketStatus::established )
                    || ( status == SocketStatus::closeWait );
        }


        SocketHandle m_handle;
    };

}

