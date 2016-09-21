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

    static constexpr uint16_t getSocketAddress(SocketHandle s, uint16_t addressOffset)
    {
        constexpr uint16_t socketRegisterBaseAddress = 0x0400;
        constexpr uint16_t socketChannelRegisterMapSize = 0x0100;
        return socketRegisterBaseAddress + s * socketChannelRegisterMapSize + addressOffset;
    }

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
        constexpr uint16_t addr = 0x0000;
        write(getSocketAddress(s, addr), value);
    }

    void W5100Device::writeSocketSourcePort(SocketHandle s, uint16_t value)
    {
        constexpr uint16_t addr = 0x0004;
        write(getSocketAddress(s, addr), byte::get<1>(value));
        write(getSocketAddress(s, addr + 1), byte::get<0>(value));
    }

    void W5100Device::writeSocketInterruptRegister(SocketHandle s, uint8_t value)
    {
        constexpr uint16_t addr = 0x0002;
        write(getSocketAddress(s, addr), value);
    }

    uint8_t W5100Device::readSocketInterruptRegister(SocketHandle s)
    {
        constexpr uint16_t addr = 0x0002;
        return read(getSocketAddress(s, addr));
    }

    void W5100Device::writeSocketCommandRegister(SocketHandle s, SocketCommand value)
    {
        constexpr uint16_t addr = 0x0001;
        write(getSocketAddress(s, addr), static_cast<uint8_t>(value));
    }

    SocketCommand W5100Device::readSocketCommandRegister(SocketHandle s)
    {
        constexpr W5100Register reg(0x0001);
        return static_cast<SocketCommand>(read(getSocketAddress(s, reg.address())));
    }

    SocketStatus W5100Device::readSocketStatusRegister(SocketHandle s)
    {
        constexpr W5100Register reg(0x0003);
        return static_cast<SocketStatus>(read(getSocketAddress(s, reg.address())));
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
            write(destAddress, buffer, std::next(buffer, transmitSize));
            auto pos = std::next(buffer, transmitSize);
            write(m_transmitBufferBaseAddress[s], pos, std::next(pos, size - transmitSize));
        }
        else
        {
            write(destAddress, buffer, std::next(buffer, size));
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
        constexpr W5100Register reg(0x0000);
        write(reg.address(), static_cast<uint8_t>(value));
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
        constexpr W5100Register reg(0x0020);
        auto b1 = read(getSocketAddress(s, reg.address()));
        auto b0 = read(getSocketAddress(s, reg.address() + 1));

        return byte::to<uint16_t>(b1, b0);
    }

    uint16_t W5100Device::readSocketTransmitWritePointer(SocketHandle s)
    {
        constexpr W5100Register reg(0x0024);
        auto b1 = read(getSocketAddress(s, reg.address()));
        auto b0 = read(getSocketAddress(s, reg.address() + 1));

        return byte::to<uint16_t>(b1, b0);
    }

    void W5100Device::writeSocketTransmitWritePointer(SocketHandle s, uint16_t value)
    {
        constexpr W5100Register reg(0x0024);
        write(getSocketAddress(s, reg.address()), byte::get<1>(value));
        write(getSocketAddress(s, reg.address() + 1), byte::get<0>(value));
    }

    void W5100Device::writeTransmitMemorySizeRegister(uint8_t value)
    {
        constexpr W5100Register reg(0x001b);
        write(reg.address(), value);
    }

    void W5100Device::writeReceiveMemorySizeRegister(uint8_t value)
    {
        constexpr W5100Register reg(0x001a);
        write(reg.address(), value);
    }

    void W5100Device::writeGatewayAddressRegister(const std::array<uint8_t, 4>& addr)
    {
        constexpr W5100Register reg(0x0001);
        write(reg.address(), addr.begin(), addr.end());
    }

    void W5100Device::writeSubnetMaskRegister(const std::array<uint8_t, 4>& addr)
    {
        constexpr W5100Register reg(0x0005);
        write(reg.address(), addr.begin(), addr.end());
    }

    void W5100Device::writeSourceMacAddressRegister(const std::array<uint8_t, 6>& addr)
    {
        constexpr W5100Register reg(0x0009);
        write(reg.address(), addr.begin(), addr.end());
    }

    void W5100Device::writeSourceIpRegister(const std::array<uint8_t, 4>& addr)
    {
        constexpr W5100Register reg(0x000f);
        write(reg.address(), addr.begin(), addr.end());
    }

    W5100Device device;

}
