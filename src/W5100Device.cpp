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
        constexpr uint16_t addr = 0x0001;
        return static_cast<SocketCommand>(read(getSocketAddress(s, addr)));
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

        if( offset + size > transmitBufferSize )
        {
            uint16_t transmitSize = transmitBufferSize - offset;
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
        spi.transfer(byte::get<1>(addr));
        spi.transfer(byte::get<0>(addr));
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
        spi.transfer(byte::get<1>(addr));
        spi.transfer(byte::get<0>(addr));
        uint8_t data = spi.transfer(0);
        spi.resetSlaveSelect();

        return data;
    }

    void W5100Device::writeModeRegister(uint8_t value)
    {
        constexpr uint16_t addr = 0x0000;
        write(addr, value);
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
        constexpr uint16_t addr = 0x0020;
        uint16_t b1 = read(getSocketAddress(s, addr));
        uint16_t b0 = read(getSocketAddress(s, addr + 1));

        return byte::from<uint16_t>(b1, b0);
    }

    uint16_t W5100Device::readSocketTransmitWritePointer(SocketHandle s)
    {
        constexpr uint16_t addr = 0x0024;
        uint16_t b1 = read(getSocketAddress(s, addr));
        uint16_t b0 = read(getSocketAddress(s, addr + 1));

        return byte::from<uint16_t>(b1, b0);
    }

    void W5100Device::writeSocketTransmitWritePointer(SocketHandle s, uint16_t value)
    {
        constexpr uint16_t addr = 0x0024;
        write(getSocketAddress(s, addr), byte::get<1>(value));
        write(getSocketAddress(s, addr + 1), byte::get<0>(value));
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

    void W5100Device::writeGatewayAddressRegister(const std::array<uint8_t, 4>& addr)
    {
        constexpr uint16_t addr_ = 0x0001;
        write(addr_, addr.data(), addr.size());
    }

    void W5100Device::writeSubnetMaskRegister(const std::array<uint8_t, 4>& addr)
    {
        constexpr uint16_t addr_ = 0x0005;
        write(addr_, addr.data(), addr.size());
    }

    void W5100Device::writeSourceMacAddressRegister(const std::array<uint8_t, 6>& addr)
    {
        constexpr uint16_t addr_ = 0x0009;
        write(addr_, addr.data(), addr.size());
    }

    void W5100Device::writeSourceIpRegister(const std::array<uint8_t, 4>& addr)
    {
        constexpr uint16_t addr_ = 0x000f;
        write(addr_, addr.data(), addr.size());
    }

    W5100Device device;

}
