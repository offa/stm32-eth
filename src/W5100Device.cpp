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

#include "W5100Device.h"
#include "W5100Registers.h"
#include "SpiWriter.h"
#include "Byte.h"

namespace eth
{
    namespace
    {

        template<size_t index, std::enable_if_t<(index < supportedSockets), int> = 0>
        constexpr uint16_t getTransmitBufferAddress()
        {
            constexpr uint16_t baseAddress = 0x4000;
            return baseAddress + W5100Device::getTransmitBufferSize() * index;
        }

        template<size_t index, std::enable_if_t<(index < supportedSockets), int> = 0>
        constexpr uint16_t getReceiveBufferAddress()
        {
            constexpr uint16_t baseAddress = 0x6000;
            return baseAddress + W5100Device::getReceiveBufferSize() * index;
        }

        constexpr std::array<uint16_t, supportedSockets> transmitBufferBaseAddress{{
                                                            getTransmitBufferAddress<0>(),
                                                            getTransmitBufferAddress<1>(),
                                                            getTransmitBufferAddress<2>(),
                                                            getTransmitBufferAddress<3>() }};
        constexpr std::array<uint16_t, supportedSockets> receiveBufferBaseAddress{{
                                                            getReceiveBufferAddress<0>(),
                                                            getReceiveBufferAddress<1>(),
                                                            getReceiveBufferAddress<2>(),
                                                            getReceiveBufferAddress<3>() }};
    }



    W5100Device::W5100Device(SpiWriter& writer) : m_writer(writer)
    {
    }

    void W5100Device::init()
    {
        writeModeRegister(Mode::reset);

        constexpr uint8_t memorySize = 0x55;
        writeTransmitMemorySizeRegister(memorySize);
        writeReceiveMemorySizeRegister(memorySize);
    }

    void W5100Device::executeSocketCommand(SocketHandle s, SocketCommand cmd)
    {
        writeSocketCommandRegister(s, cmd);

        while( readSocketCommandRegister(s) != SocketCommand::executed )
        {
            // Wait for completion
        }
    }

    void W5100Device::writeSocketModeRegister(SocketHandle s, uint8_t value)
    {
        write(w5100::socketMode(s), value);
    }

    void W5100Device::writeSocketSourcePort(SocketHandle s, uint16_t value)
    {
        write(w5100::socketSourcePort(s), value);
    }

    void W5100Device::writeSocketInterruptRegister(SocketHandle s, SocketInterrupt value)
    {
        write(w5100::socketInterrupt(s), value.value());
    }

    SocketInterrupt W5100Device::readSocketInterruptRegister(SocketHandle s)
    {
        return SocketInterrupt(read(w5100::socketInterrupt(s)));
    }

    void W5100Device::writeSocketCommandRegister(SocketHandle s, SocketCommand value)
    {
        write(w5100::socketCommand(s), static_cast<uint8_t>(value));
    }

    SocketCommand W5100Device::readSocketCommandRegister(SocketHandle s)
    {
        return static_cast<SocketCommand>(read(w5100::socketCommand(s)));
    }

    SocketStatus W5100Device::readSocketStatusRegister(SocketHandle s)
    {
        return static_cast<SocketStatus>(read(w5100::socketStatus(s)));
    }

    uint16_t W5100Device::getTransmitFreeSize(SocketHandle s)
    {
        return readFreesize(w5100::socketTransmitFreeSize(s));
    }


    uint16_t W5100Device::getReceiveFreeSize(SocketHandle s)
    {
        return readFreesize(w5100::socketReceiveFreeSize(s));
    }

    uint16_t W5100Device::readFreesize(W5100Register freesizeReg)
    {
        uint16_t val = 0;
        uint16_t val1 = 0;

        do
        {
            val1 = readWord(freesizeReg);

            if( val1 != 0 )
            {
                val = readWord(freesizeReg);
            }
        }
        while( val != val1 );

        return val;
    }

    void W5100Device::sendData(SocketHandle s, const gsl::span<const uint8_t> buffer)
    {
        constexpr uint16_t transmitBufferMask = 0x07ff;
        const auto size = buffer.length();
        const uint16_t writePointer = readSocketTransmitWritePointer(s);
        const uint16_t offset = writePointer & transmitBufferMask;
        const uint16_t destAddress = offset + transmitBufferBaseAddress[s];

        if( offset + size > transmitBufferSize )
        {
            const uint16_t transmitSize = transmitBufferSize - offset;
            const auto remaining = size - transmitSize;

            write(W5100Register(destAddress, transmitSize), buffer.first(transmitSize));
            write(W5100Register(transmitBufferBaseAddress[s], remaining), buffer.last(remaining));
        }
        else
        {
            write(W5100Register(destAddress, size), buffer);
        }

        writeSocketTransmitWritePointer(s, (writePointer + size));
    }

    uint16_t W5100Device::receiveData(SocketHandle s, gsl::span<uint8_t> buffer)
    {
        constexpr uint16_t receiveBufferMask = 0x07ff;
        const auto size = buffer.length();
        const uint16_t readPointer = readSocketReceiveReadPointer(s);
        const uint16_t offset = readPointer & receiveBufferMask;
        const uint16_t destAddress = offset + receiveBufferBaseAddress[s];
        const auto reg = makeRegister<uint8_t>(destAddress);

        if( offset + size > receiveBufferSize )
        {
            const auto first = receiveBufferSize - offset;
            auto border = buffer.first(first);
            auto end = buffer.last(size - first);

            read(reg, border);
            read(reg, end);
        }
        else
        {
            read(reg, buffer);
        }

        writeSocketReceiveReadPointer(s, (readPointer + size));
        return size;
    }

    void W5100Device::write(uint16_t addr, uint16_t offset, uint8_t data)
    {
        const auto address = addr + offset;
        m_writer.write(address, data);
    }

    void W5100Device::write(W5100Register reg, uint8_t data)
    {
        write(reg.address(), 0, data);
    }

    void W5100Device::write(W5100Register reg, uint16_t data)
    {
        write(reg.address(), 0, byte::get<1>(data));
        write(reg.address(), 1, byte::get<0>(data));
    }

    void W5100Device::write(W5100Register reg, const gsl::span<const uint8_t> buffer)
    {
        uint16_t offset = 0;
        std::for_each(buffer.cbegin(), buffer.cend(), [&](uint8_t data)
        {
            write(reg.address(), offset++, data);
        });
    }

    uint8_t W5100Device::read(uint16_t addr, uint16_t offset)
    {
        const auto address = addr + offset;
        const auto data = m_writer.read(address);

        return data;
    }

    uint8_t W5100Device::read(W5100Register reg)
    {
        return read(reg.address(), 0);
    }

    uint16_t W5100Device::readWord(W5100Register reg)
    {
        const auto byte1 = read(reg);
        const auto byte0 = read(reg.address(), 1);
        return byte::to<uint16_t>(byte1, byte0);
    }

    uint16_t W5100Device::read(W5100Register reg, gsl::span<uint8_t> buffer)
    {
        uint16_t offset = 0;
        std::generate(buffer.begin(), buffer.end(), [&]
        {
            return read(reg.address(), offset++);
        });

        return offset;

    }

    void W5100Device::writeModeRegister(Mode value)
    {
        write(w5100::mode, static_cast<uint8_t>(value));
    }

    void W5100Device::setGatewayAddress(std::array<uint8_t, 4> addr)
    {
        write(w5100::gatewayAddress, addr);
    }

    void W5100Device::setSubnetMask(std::array<uint8_t, 4> addr)
    {
        write(w5100::subnetMask, addr);
    }

    void W5100Device::setMacAddress(std::array<uint8_t, 6> addr)
    {
        write(w5100::sourceMacAddress, addr);
    }

    void W5100Device::setIpAddress(std::array<uint8_t, 4> addr)
    {
        write(w5100::sourceIpAddress, addr);
    }

    void W5100Device::setDestIpAddress(SocketHandle s, std::array<uint8_t, 4> addr)
    {
        write(w5100::socketDestIpAddress(s), addr);
    }

    void W5100Device::setDestPort(SocketHandle s, uint16_t port)
    {
        write(w5100::socketDestPort(s), port);
    }

    uint16_t W5100Device::readSocketTransmitWritePointer(SocketHandle s)
    {
        return readWord(w5100::socketTransmitWritePointer(s));
    }

    void W5100Device::writeSocketTransmitWritePointer(SocketHandle s, uint16_t value)
    {
        write(w5100::socketTransmitWritePointer(s), value);
    }

    uint16_t W5100Device::readSocketReceiveReadPointer(SocketHandle s)
    {
        return readWord(w5100::socketReceiveReadPointer(s));
    }

    void W5100Device::writeSocketReceiveReadPointer(SocketHandle s, uint16_t value)
    {
        write(w5100::socketReceiveReadPointer(s), value);
    }

    void W5100Device::writeTransmitMemorySizeRegister(uint8_t value)
    {
        write(w5100::transmitMemorySize, value);
    }

    void W5100Device::writeReceiveMemorySizeRegister(uint8_t value)
    {
        write(w5100::receiveMemorySize, value);
    }

}
