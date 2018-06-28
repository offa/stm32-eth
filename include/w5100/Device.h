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

#pragma once

#include "SocketHandle.h"
#include "SocketCommand.h"
#include "SocketStatus.h"
#include "SocketInterrupt.h"
#include "Mode.h"
#include "NetConfig.h"
#include "w5100/Register.h"
#include "Byte.h"
#include <algorithm>
#include <iterator>
#include <cstdint>
#include <gsl/span>

namespace eth::spi
{
    class SpiWriter;
}

namespace eth::w5100
{

    class Device
    {
    public:

        explicit Device(spi::SpiWriter& writer);
        Device(Device&&) = default;


        void executeSocketCommand(SocketHandle s, SocketCommand cmd);

        void writeSocketModeRegister(SocketHandle s, std::uint8_t value);
        void writeSocketSourcePort(SocketHandle s, std::uint16_t value);

        void writeSocketInterruptRegister(SocketHandle s, SocketInterrupt value);
        SocketInterrupt readSocketInterruptRegister(SocketHandle s);

        void writeSocketCommandRegister(SocketHandle s, SocketCommand value);
        SocketCommand readSocketCommandRegister(SocketHandle s);

        SocketStatus readSocketStatusRegister(SocketHandle s);

        std::uint16_t getTransmitFreeSize(SocketHandle s);
        std::uint16_t getReceiveFreeSize(SocketHandle s);

        void sendData(SocketHandle s, const gsl::span<const std::uint8_t> buffer);
        std::uint16_t receiveData(SocketHandle s, gsl::span<std::uint8_t> buffer);

        template<class T, std::size_t n = sizeof(T),
                std::enable_if_t<(n > 1) && (n <= sizeof(T)), int> = 0,
                std::enable_if_t<std::is_integral_v<T>, int> = 0>
        void write(Register<T> reg, T data)
        {
            constexpr auto pos{n - 1};
            write(reg.address(), sizeof(T) - n, byte::get<pos>(data));
            write<T, pos>(reg, data);
        }

        template<class T, std::size_t n = sizeof(T),
                std::enable_if_t<(n <= 1), int> = 0,
                std::enable_if_t<std::is_integral_v<T>, int> = 0>
        void write(Register<T> reg, T data)
        {
            write(reg.address(), sizeof(T) - n, byte::get<(n - 1)>(data));
        }

        template<class T, class Iterator>
        void write(Register<T> reg, Iterator begin, Iterator end)
        {
            static_assert(byte::is_byte_compatible_itr_v<Iterator>, "Invalid Type");

            std::uint16_t offset{0};
            std::for_each(begin, end, [this, &reg, &offset](std::uint8_t data)
            {
                write(reg.address(), offset++, data);
            });
        }

        template<class T, std::size_t n = sizeof(T),
                std::enable_if_t<(n > 1) && (n <= sizeof(T)), int> = 0,
                std::enable_if_t<std::is_integral_v<T>, int> = 0>
        T read(Register<T> reg)
        {
            constexpr auto pos{sizeof(T) - n};
            const auto byte0 = read(reg.address(), pos);
            const auto byte1 = read<T, (pos + 1)>(reg);
            return byte::to<T>(byte0, byte1);
        }

        template<class T, std::size_t n = sizeof(T),
                std::enable_if_t<(n <= 1), int> = 0,
                std::enable_if_t<std::is_integral_v<T>, int> = 0>
        T read(Register<T> reg)
        {
            return read(reg.address(), sizeof(T) - n);
        }

        template<class T, class Iterator>
        auto read(Register<T> reg, Iterator begin, Iterator end)
        {
            static_assert(byte::is_byte_compatible_itr_v<Iterator>, "Invalid Type");

            std::size_t offset{0};
            std::generate(begin, end, [this, &reg, &offset]
            {
                return read(reg.address(), offset++);
            });

            return offset;
        }

        void writeModeRegister(Mode value);

        void setDestAddress(SocketHandle s, NetAddress<4> addr, std::uint16_t port);


        static constexpr std::uint16_t getTransmitBufferSize() noexcept
        {
            return transmitBufferSize;
        }

        static constexpr std::uint16_t getReceiveBufferSize() noexcept
        {
            return receiveBufferSize;
        }


        Device& operator=(Device&&) = default;


    private:

        void write(std::uint16_t addr, std::uint16_t offset, std::uint8_t data);
        std::uint8_t read(std::uint16_t addr, std::uint16_t offset);

        std::uint16_t readFreesize(Register<std::uint16_t> freesizeReg);


        spi::SpiWriter& m_writer;
        static inline constexpr std::uint16_t transmitBufferSize{2048};
        static inline constexpr std::uint16_t receiveBufferSize = transmitBufferSize;
    };


    void setupDevice(Device& dev, eth::NetConfig config);

}

