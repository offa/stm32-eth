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
namespace w5100
{

    namespace
    {
        constexpr uint16_t toTransmitBufferAddress(SocketHandle s)
        {
            constexpr uint16_t baseAddress = 0x4000;
            return baseAddress + ( Device::getTransmitBufferSize() * s.get() );
        }

        constexpr uint16_t toReceiveBufferAddress(SocketHandle s)
        {
            constexpr uint16_t baseAddress = 0x6000;
            return baseAddress + ( Device::getReceiveBufferSize() * s.get() );
        }
    }



    Device::Device(SpiWriter& writer) : m_writer(writer)
    {
        writeModeRegister(Mode::reset);

        constexpr uint8_t memorySize = 0x55;
        write(w5100::transmitMemorySize, memorySize);
        write(w5100::receiveMemorySize, memorySize);
    }

    void Device::executeSocketCommand(SocketHandle s, SocketCommand cmd)
    {
        writeSocketCommandRegister(s, cmd);

        while( readSocketCommandRegister(s) != SocketCommand::executed )
        {
            // Wait for completion
        }
    }

    void Device::writeSocketModeRegister(SocketHandle s, uint8_t value)
    {
        write(w5100::socketMode(s), value);
    }

    void Device::writeSocketSourcePort(SocketHandle s, uint16_t value)
    {
        write(w5100::socketSourcePort(s), value);
    }

    void Device::writeSocketInterruptRegister(SocketHandle s, SocketInterrupt value)
    {
        write(w5100::socketInterrupt(s), value.value());
    }

    SocketInterrupt Device::readSocketInterruptRegister(SocketHandle s)
    {
        return static_cast<SocketInterrupt>(read(w5100::socketInterrupt(s)));
    }

    void Device::writeSocketCommandRegister(SocketHandle s, SocketCommand value)
    {
        write(w5100::socketCommand(s), static_cast<uint8_t>(value));
    }

    SocketCommand Device::readSocketCommandRegister(SocketHandle s)
    {
        return static_cast<SocketCommand>(read(w5100::socketCommand(s)));
    }

    SocketStatus Device::readSocketStatusRegister(SocketHandle s)
    {
        return static_cast<SocketStatus>(read(w5100::socketStatus(s)));
    }

    uint16_t Device::getTransmitFreeSize(SocketHandle s)
    {
        return readFreesize(w5100::socketTransmitFreeSize(s));
    }

    uint16_t Device::getReceiveFreeSize(SocketHandle s)
    {
        return readFreesize(w5100::socketReceiveFreeSize(s));
    }

    uint16_t Device::readFreesize(W5100Register<uint16_t> freesizeReg)
    {
        uint16_t firstRead = 0;
        uint16_t secondRead = 0;

        do
        {
            firstRead = readWord(freesizeReg);

            if( firstRead != 0 )
            {
                secondRead = readWord(freesizeReg);
            }
        }
        while( secondRead != firstRead );

        return secondRead;
    }

    void Device::sendData(SocketHandle s, const gsl::span<const uint8_t> buffer)
    {
        constexpr uint16_t transmitBufferMask = 0x07ff;
        const auto size = buffer.length();
        const uint16_t writePointer = readWord(w5100::socketTransmitWritePointer(s));
        const uint16_t offset = writePointer & transmitBufferMask;
        const uint16_t destAddress = offset + toTransmitBufferAddress(s);

        if( offset + size > transmitBufferSize )
        {
            const uint16_t first = transmitBufferSize - offset;
            const auto border = std::next(buffer.cbegin(), first);
            write(makeRegister<gsl::span<const uint8_t>>(destAddress), buffer.cbegin(), border);
            write(makeRegister<gsl::span<const uint8_t>>(toTransmitBufferAddress(s)), border, buffer.cend());
        }
        else
        {
            write(W5100Register<gsl::span<const uint8_t>>(destAddress), buffer.cbegin(), buffer.cend());
        }

        write(w5100::socketTransmitWritePointer(s), static_cast<uint16_t>(writePointer + size));
    }

    uint16_t Device::receiveData(SocketHandle s, gsl::span<uint8_t> buffer)
    {
        constexpr uint16_t receiveBufferMask = 0x07ff;
        const auto size = buffer.length();
        const uint16_t readPointer = readWord(w5100::socketReceiveReadPointer(s));
        const uint16_t offset = readPointer & receiveBufferMask;
        const uint16_t destAddress = offset + toReceiveBufferAddress(s);
        const auto reg = makeRegister<gsl::span<uint8_t>>(destAddress);

        if( offset + size > receiveBufferSize )
        {
            const auto first = receiveBufferSize - offset;
            auto border = std::next(buffer.begin(), first);

            read(reg, buffer.begin(), border);
            read(reg, border, buffer.end());
        }
        else
        {
            read(reg, buffer.begin(), buffer.end());
        }

        write(w5100::socketReceiveReadPointer(s), static_cast<uint16_t>(readPointer + size));

        return size;
    }

    void Device::write(uint16_t addr, uint16_t offset, uint8_t data)
    {
        m_writer.write(addr + offset, data);
    }

    void Device::write(W5100Register<uint8_t> reg, uint8_t data)
    {
        write(reg.address(), 0, data);
    }

    void Device::write(W5100Register<uint16_t> reg, uint16_t data)
    {
        write(reg.address(), 0, byte::get<1>(data));
        write(reg.address(), 1, byte::get<0>(data));
    }

    uint8_t Device::read(uint16_t addr, uint16_t offset)
    {
        return m_writer.read(addr + offset);
    }

    uint8_t Device::read(W5100Register<uint8_t> reg)
    {
        return read(reg.address(), 0);
    }

    uint16_t Device::readWord(W5100Register<uint16_t> reg)
    {
        const auto byte1 = read(reg.address(), 0);
        const auto byte0 = read(reg.address(), 1);

        return byte::to<uint16_t>(byte1, byte0);
    }

    void Device::writeModeRegister(Mode value)
    {
        write(w5100::mode, static_cast<uint8_t>(value));
    }

    void Device::setGatewayAddress(std::array<uint8_t, 4> addr)
    {
        write(w5100::gatewayAddress, addr.cbegin(), addr.cend());
    }

    void Device::setSubnetMask(std::array<uint8_t, 4> addr)
    {
        write(w5100::subnetMask, addr.cbegin(), addr.cend());
    }

    void Device::setMacAddress(std::array<uint8_t, 6> addr)
    {
        write(w5100::sourceMacAddress, addr.cbegin(), addr.cend());
    }

    void Device::setIpAddress(std::array<uint8_t, 4> addr)
    {
        write(w5100::sourceIpAddress, addr.cbegin(), addr.cend());
    }

    void Device::setDestIpAddress(SocketHandle s, std::array<uint8_t, 4> addr)
    {
        write(w5100::socketDestIpAddress(s), addr.cbegin(), addr.cend());
    }

    void Device::setDestPort(SocketHandle s, uint16_t port)
    {
        write(w5100::socketDestPort(s), port);
    }

}
}
