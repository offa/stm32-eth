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

#ifndef SOCKET_H
#define SOCKET_H

#include <stdint.h>
#include "SocketType.h"

namespace eth
{

    enum class SocketMode
    {
        tcp = 0x01,
        udp = 0x02,
        ipraw = 0x03,
        macraw = 0x04,
        pppoe = 0x05
    };

    enum class SocketStatus
    {
        closed = 0x00,
        init = 0x13,
        listen = 0x14,
        synSent = 0x15,
        synRecv = 0x16,
        established = 0x17,
        finWait = 0x18,
        closing = 0x1a,
        timeWait = 0x1b,
        closeWait = 0x1c,
        lastAck = 0x1d,
        udp = 0x22,
        ipraw = 0x32,
        macraw = 0x42,
        pppoe = 0x5f
    };

    enum class SocketCommand
    {
        open = 0x01,
        listen = 0x02,
        connect = 0x04,
        disconnect = 0x08,
        close = 0x10,
        send = 0x20,
        sendMac = 0x21,
        sendKeep = 0x22,
        recv = 0x40
    };

    enum class SocketInterrupt
    {
        sendOk = 0x10,
        timeout = 0x08,
        recv = 0x04,
        disconnect = 0x02,
        connect = 0x01
    };

    void close(Socket s);
    uint8_t socket(Socket s, uint8_t protocol, uint16_t port, uint8_t flag);
    uint8_t listen(Socket s);
    uint16_t send(Socket s, const uint8_t* buf, uint16_t len);

}

#endif /* SOCKET_H */
