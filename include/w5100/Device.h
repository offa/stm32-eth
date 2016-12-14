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
#include "w5100/Register.h"
#include "Byte.h"
#include <array>
#include <algorithm>
#include <iterator>
#include <stdint.h>
#include <gsl/span>

namespace eth
{
    class SpiWriter;


    namespace w5100
    {

        class Device
        {
        public:

            explicit Device(SpiWriter& writer);
            Device(Device&&) = default;


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

            // TODO: Limit T to unsigned integral types
            template<class T, size_t n = sizeof(T),
                std::enable_if_t<(n > 0), int> = 0>
            void write(Register<T> reg, T data)
            {
                write(reg.address(), sizeof(T) - n, byte::get<n-1>(data));
                write<T, n-1>(reg, data);
            }

            template<class T, size_t n = sizeof(T),
                std::enable_if_t<(n == 0), int> = 0>
            void write(Register<T> reg, T data)
            {
                // TODO: Cleanup
                (void) reg;
                (void) data;
            }

            template<class T, class Iterator>
            void write(Register<T> reg, Iterator begin, Iterator end)
            {
                uint16_t offset = 0;
                std::for_each(begin, end, [&](uint8_t data)
                {
                    write(reg.address(), offset++, data);
                });

            }

            uint8_t read(Register<uint8_t> reg);
            uint16_t readWord(Register<uint16_t> reg);

            template<class T, class Iterator>
            uint16_t read(Register<T> reg, Iterator begin, Iterator end)
            {
                uint16_t offset = 0;
                std::generate(begin, end, [&]
                {
                    return read(reg.address(), offset++);
                });

                return offset;
            }

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


            Device& operator=(Device&&) = default;


        private:

            void write(uint16_t addr, uint16_t offset, uint8_t data);
            uint8_t read(uint16_t addr, uint16_t offset);

            uint16_t readFreesize(Register<uint16_t> freesizeReg);


            SpiWriter& m_writer;
            static constexpr uint16_t transmitBufferSize = 2048;
            static constexpr uint16_t receiveBufferSize = transmitBufferSize;
        };

    }
}

