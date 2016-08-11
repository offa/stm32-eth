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

#ifndef SOCKETSTATUS_H
#define SOCKETSTATUS_H

namespace eth
{

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
        ipRaw = 0x32,
        macRaw = 0x42,
        pppoe = 0x5f
    };

}

#endif /* SOCKETSTATUS_H */
