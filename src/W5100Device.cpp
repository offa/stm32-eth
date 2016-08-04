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
#include <algorithm>

namespace eth
{

    static constexpr uint16_t getSocketAddress(SocketHandle s, uint16_t addressOffset)
    {
        constexpr uint16_t socketRegisterBaseAddress = 0x0400;
        constexpr uint16_t socketChannelRegisterMapSize = 0x0100;

        return socketRegisterBaseAddress + s * socketChannelRegisterMapSize + addressOffset;
    }


    W5100Device::W5100Device()
    {
        uint8_t index = 0;
        std::generate(m_transmitBufferBaseAddress.begin(), m_transmitBufferBaseAddress.end(), [&]
        {
            constexpr uint16_t baseAddress = 0x4000;
            return baseAddress + transmitBufferSize * (index++);
        });
    }

    void W5100Device::init()
    {
        constexpr uint8_t resetBit = 7;
        writeModeRegister(1 << resetBit);

        constexpr uint8_t memorySize = 0x55;
        writeTransmitMemorySizeRegister(memorySize);
        writeReceiveMemorySizeRegister(memorySize);
    }

    void W5100Device::executeSocketCommand(SocketHandle s, SocketCommand cmd)
    {
        writeSocketCommandRegister(s, static_cast<uint8_t>(cmd));

        while( readSocketCommandRegister(s) != 0 )
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
        write(getSocketAddress(s, addr), value >> 8);
        write(getSocketAddress(s, addr + 1), value & 0xff);
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

    void W5100Device::writeSocketCommandRegister(SocketHandle s, uint8_t value)
    {
        constexpr uint16_t addr = 0x0001;
        write(getSocketAddress(s, addr), value);
    }

    uint8_t W5100Device::readSocketCommandRegister(SocketHandle s)
    {
        constexpr uint16_t addr = 0x0001;
        return read(getSocketAddress(s, addr));
    }

    uint8_t W5100Device::readSocketStatusRegister(SocketHandle s)
    {
        constexpr uint16_t addr = 0x0003;
        return read(getSocketAddress(s, addr));
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
        constexpr uint16_t bufferSize = transmitBufferSize;

        if( offset + size > bufferSize )
        {
            uint16_t transmitSize = bufferSize - offset;
            write(destAddress, buffer, transmitSize);
            write(m_transmitBufferBaseAddress[s], buffer + transmitSize, size - transmitSize);
        }
        else
        {
            write(destAddress, buffer, size);
        }

        writePointer += size;
        writeSocketTransmitWritePointer(s, writePointer);
    }

    void W5100Device::write(uint16_t addr, uint8_t data)
    {
        spi.setSlaveSelect();
        spi.transfer(opcodeWrite);
        spi.transfer(addr >> 8);
        spi.transfer(addr & 0xff);
        spi.transfer(data);
        spi.resetSlaveSelect();
    }

    void W5100Device::write(uint16_t addr, const uint8_t* buffer, uint16_t size)
    {
        std::for_each(buffer, buffer + size, [&](uint8_t data)
        {
            write(addr, data);
            ++addr;
        });
    }

    uint8_t W5100Device::read(uint16_t addr)
    {
        spi.setSlaveSelect();
        spi.transfer(opcodeRead);
        spi.transfer(addr >> 8);
        spi.transfer(addr & 0xff);
        uint8_t data = spi.transfer(0);
        spi.resetSlaveSelect();

        return data;
    }

    void W5100Device::writeModeRegister(uint8_t value)
    {
        constexpr uint16_t addr = 0x0000;
        write(addr, value);
    }

    void W5100Device::setGatewayAddress(uint8_t* addr)
    {
        writeGatewayAddressRegister(addr);
    }

    void W5100Device::setSubnetMask(uint8_t* addr)
    {
        writeSubnetMaskRegister(addr);
    }

    void W5100Device::setMacAddress(uint8_t* addr)
    {
        writeSourceMacAddressRegister(addr);
    }

    void W5100Device::setIpAddress(uint8_t* addr)
    {
        writeSourceIpRegister(addr);
    }

    uint16_t W5100Device::readSocketTransmitFreeSizeRegister(SocketHandle s)
    {
        constexpr uint16_t addr = 0x0020;
        uint16_t b1 = read(getSocketAddress(s, addr));
        uint16_t b2 = read(getSocketAddress(s, addr + 1));
        b1 = b1 << 8;
        b2 = b2 & 0xff;
        b1 = b1 | b2;

        return b1;
    }

    uint16_t W5100Device::readSocketTransmitWritePointer(SocketHandle s)
    {
        constexpr uint16_t addr = 0x0024;
        uint16_t b1 = read(getSocketAddress(s, addr));
        uint16_t b2 = read(getSocketAddress(s, addr + 1));
        b1 = b1 << 8;
        b2 = b2 & 0xff;
        b1 = b1 | b2;

        return b1;
    }

    void W5100Device::writeSocketTransmitWritePointer(SocketHandle s, uint16_t value)
    {
        constexpr uint16_t addr = 0x0024;
        write(getSocketAddress(s, addr), value >> 8);
        write(getSocketAddress(s, addr + 1), value & 0xff);
    }

    void W5100Device::writeTransmitMemorySizeRegister(uint8_t value)
    {
        constexpr uint16_t addr = 0x001b;
        write(addr, value);
    }

    void W5100Device::writeReceiveMemorySizeRegister(uint8_t value)
    {
        constexpr uint16_t addr = 0x001a;
        write(addr, value);
    }

    void W5100Device::writeGatewayAddressRegister(uint8_t* addr)
    {
        constexpr uint16_t addr_ = 0x0001;
        write(addr_, addr, 4);
    }

    void W5100Device::writeSubnetMaskRegister(uint8_t* addr)
    {
        constexpr uint16_t addr_ = 0x0005;
        write(addr_, addr, 4);
    }

    void W5100Device::writeSourceMacAddressRegister(uint8_t* addr)
    {
        constexpr uint16_t addr_ = 0x0009;
        write(addr_, addr, 6);
    }

    void W5100Device::writeSourceIpRegister(uint8_t* addr)
    {
        constexpr uint16_t addr_ = 0x000f;
        write(addr_, addr, 4);
    }

    W5100Device device;

}
