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
#include "SocketCommand.h"
#include "SocketStatus.h"
#include "SocketInterrupt.h"
#include "Mode.h"
#include "W5100Register.h"
#include <array>
#include <algorithm>
#include <iterator>
#include <stdint.h>
#include <gsl/span>

namespace eth
{
    class SpiWriter;


    class W5100Device
    {
    public:

        explicit W5100Device(SpiWriter& writer);
        W5100Device(W5100Device&&) = default;


        void executeSocketCommand(SocketHandle s, SocketCommand cmd);

        void writeSocketModeRegister(SocketHandle s, uint8_t value);
        void writeSocketSourcePort(SocketHandle s, uint16_t value);

        void writeSocketInterruptRegister(SocketHandle s, SocketInterrupt value);
        SocketInterrupt readSocketInterruptRegister(SocketHandle s);

        void writeSocketCommandRegister(SocketHandle s, SocketCommand value);
        SocketCommand readSocketCommandRegister(SocketHandle s);

        SocketStatus readSocketStatusRegister(SocketHandle s);

        uint16_t getTransmitFreeSize(SocketHandle s);
        uint16_t getReceiveFreeSize(SocketHandle s);

        void sendData(SocketHandle s, const gsl::span<const uint8_t> buffer);
        uint16_t receiveData(SocketHandle s, gsl::span<uint8_t> buffer);

        void write(W5100Register reg, uint8_t data);
        void write(W5100Register reg, uint16_t data);
        void write(W5100Register reg, const gsl::span<const uint8_t> buffer);

        uint8_t read(W5100Register reg);
        uint16_t readWord(W5100Register reg);
        uint16_t read(W5100Register reg, gsl::span<uint8_t> buffer);

        void writeModeRegister(Mode value);

        void setGatewayAddress(std::array<uint8_t, 4> addr);
        void setSubnetMask(std::array<uint8_t, 4> addr);
        void setMacAddress(std::array<uint8_t, 6> addr);
        void setIpAddress(std::array<uint8_t, 4> addr);

        void setDestIpAddress(SocketHandle s, std::array<uint8_t, 4> addr);
        void setDestPort(SocketHandle s, uint16_t port);


        static constexpr uint16_t getTransmitBufferSize()
        {
            return transmitBufferSize;
        }

        static constexpr uint16_t getReceiveBufferSize()
        {
            return receiveBufferSize;
        }


        W5100Device& operator=(W5100Device&&) = default;


    private:

        void init();

        void write(uint16_t addr, uint16_t offset, uint8_t data);
        uint8_t read(uint16_t addr, uint16_t offset);

        uint16_t readFreesize(W5100Register freesizeReg);


        SpiWriter& m_writer;
        static constexpr uint16_t transmitBufferSize = 2048;
        static constexpr uint16_t receiveBufferSize = transmitBufferSize;
    };

}

