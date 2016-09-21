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
#include "Spi.h"
#include "Byte.h"

namespace eth
{

    template<size_t index, std::enable_if_t<(index < supportedSockets), int> = 0>
    static constexpr uint16_t getBufferAddress(uint16_t bufferSize)
    {
        constexpr uint16_t baseAddress = 0x4000;
        return baseAddress + bufferSize * index;
    }


    W5100Device::W5100Device() : m_transmitBufferBaseAddress({{ getBufferAddress<0>(transmitBufferSize),
                                                                getBufferAddress<1>(transmitBufferSize),
                                                                getBufferAddress<2>(transmitBufferSize),
                                                                getBufferAddress<3>(transmitBufferSize) }})
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

        while( readSocketCommandRegister(s) != SocketCommand::commandExecuted )
        {
            // Wait for completion
        }
    }

    void W5100Device::writeSocketModeRegister(SocketHandle s, uint8_t value)
    {
        write(socketMode(s), value);
    }

    void W5100Device::writeSocketSourcePort(SocketHandle s, uint16_t value)
    {
        write(socketSourcePort(s), byte::get<1>(value));
        write(socketSourcePort(s).address() + 1, byte::get<0>(value));
    }

    void W5100Device::writeSocketInterruptRegister(SocketHandle s, uint8_t value)
    {
        write(socketInterrupt(s), value);
    }

    uint8_t W5100Device::readSocketInterruptRegister(SocketHandle s)
    {
        return read(socketInterrupt(s));
    }

    void W5100Device::writeSocketCommandRegister(SocketHandle s, SocketCommand value)
    {
        write(socketCommand(s), static_cast<uint8_t>(value));
    }

    SocketCommand W5100Device::readSocketCommandRegister(SocketHandle s)
    {
        return static_cast<SocketCommand>(read(socketCommand(s)));
    }

    SocketStatus W5100Device::readSocketStatusRegister(SocketHandle s)
    {
        return static_cast<SocketStatus>(read(socketStatus(s)));
    }

    uint16_t W5100Device::getTransmitFreeSize(SocketHandle s)
    {
        uint16_t val = 0;
        uint16_t val1 = 0;

        do
        {
            val1 = readSocketTransmitFreeSizeRegister(s);

            if( val1 != 0 )
            {
                val = readSocketTransmitFreeSizeRegister(s);
            }
        }
        while( val != val1 );

        return val;
    }

    void W5100Device::sendData(SocketHandle s, const uint8_t* buffer, uint16_t size)
    {
        constexpr uint16_t transmitBufferMask = 0x07ff;
        uint16_t writePointer = readSocketTransmitWritePointer(s);
        uint16_t offset = writePointer & transmitBufferMask;
        uint16_t destAddress = offset + m_transmitBufferBaseAddress[s];

        if( offset + size > transmitBufferSize )
        {
            uint16_t transmitSize = transmitBufferSize - offset;
            write(W5100Register(destAddress, transmitSize), buffer, std::next(buffer, transmitSize));
            auto pos = std::next(buffer, transmitSize);
            auto remaining = size - transmitSize;
            write(W5100Register(m_transmitBufferBaseAddress[s], remaining), pos, std::next(pos, remaining));
        }
        else
        {
            write(W5100Register(destAddress, size), buffer, std::next(buffer, size));
        }

        writePointer += size;
        writeSocketTransmitWritePointer(s, writePointer);
    }

    void W5100Device::write(uint16_t addr, uint8_t data)
    {
        spi.setSlaveSelect();
        spi.transmit(opcodeWrite);
        spi.transmit(byte::get<1>(addr));
        spi.transmit(byte::get<0>(addr));
        spi.transmit(data);
        spi.resetSlaveSelect();
    }

    uint8_t W5100Device::read(uint16_t addr)
    {
        spi.setSlaveSelect();
        spi.transmit(opcodeRead);
        spi.transmit(byte::get<1>(addr));
        spi.transmit(byte::get<0>(addr));
        auto data = spi.receive();
        spi.resetSlaveSelect();

        return data;
    }

    void W5100Device::writeModeRegister(Mode value)
    {
        write(mode, static_cast<uint8_t>(value));
    }

    void W5100Device::setGatewayAddress(const std::array<uint8_t, 4>& addr)
    {
        writeGatewayAddressRegister(addr);
    }

    void W5100Device::setSubnetMask(const std::array<uint8_t, 4>& addr)
    {
        writeSubnetMaskRegister(addr);
    }

    void W5100Device::setMacAddress(const std::array<uint8_t, 6>& addr)
    {
        writeSourceMacAddressRegister(addr);
    }

    void W5100Device::setIpAddress(const std::array<uint8_t, 4>& addr)
    {
        writeSourceIpRegister(addr);
    }

    uint16_t W5100Device::readSocketTransmitFreeSizeRegister(SocketHandle s)
    {
        auto b1 = read(socketTransmitFreeSize(s));
        auto b0 = read(socketTransmitFreeSize(s).address() + 1);

        return byte::to<uint16_t>(b1, b0);
    }

    uint16_t W5100Device::readSocketTransmitWritePointer(SocketHandle s)
    {
        auto b1 = read(socketTransmitWritePointer(s));
        auto b0 = read(socketTransmitWritePointer(s).address() + 1);

        return byte::to<uint16_t>(b1, b0);
    }

    void W5100Device::writeSocketTransmitWritePointer(SocketHandle s, uint16_t value)
    {
        write(socketTransmitWritePointer(s), byte::get<1>(value));
        write(socketTransmitWritePointer(s).address() + 1, byte::get<0>(value));
    }

    void W5100Device::writeTransmitMemorySizeRegister(uint8_t value)
    {
        write(transmitMemorySize, value);
    }

    void W5100Device::writeReceiveMemorySizeRegister(uint8_t value)
    {
        write(receiveMemorySize, value);
    }

    void W5100Device::writeGatewayAddressRegister(const std::array<uint8_t, 4>& addr)
    {
        write(gatewayAddress, addr.begin(), addr.end());
    }

    void W5100Device::writeSubnetMaskRegister(const std::array<uint8_t, 4>& addr)
    {
        write(subnetMask, addr.begin(), addr.end());
    }

    void W5100Device::writeSourceMacAddressRegister(const std::array<uint8_t, 6>& addr)
    {
        write(sourceMacAddress, addr.begin(), addr.end());
    }

    void W5100Device::writeSourceIpRegister(const std::array<uint8_t, 4>& addr)
    {
        write(sourceIpAddress, addr.begin(), addr.end());
    }

    W5100Device device;

}
