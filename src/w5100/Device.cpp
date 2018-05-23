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

#include "w5100/Device.h"
#include "w5100/Registers.h"
#include "spi/SpiWriter.h"

namespace eth
{
namespace w5100
{

    namespace
    {
        constexpr std::uint16_t toTransmitBufferAddress(SocketHandle s)
        {
            constexpr std::uint16_t baseAddress{0x4000};
            return baseAddress + ( Device::getTransmitBufferSize() * s.value() );
        }

        constexpr std::uint16_t toReceiveBufferAddress(SocketHandle s)
        {
            constexpr std::uint16_t baseAddress{0x6000};
            return baseAddress + ( Device::getReceiveBufferSize() * s.value() );
        }

        template<std::size_t limit>
        constexpr bool isWrapAround(std::size_t offset, std::size_t size)
        {
            return ( offset + size ) > limit;
        }

    }



    Device::Device(spi::SpiWriter& writer) : m_writer(writer)
    {
        writeModeRegister(Mode::reset);

        constexpr std::uint8_t memorySize{0x55};
        write(registers::transmitMemorySize, memorySize);
        write(registers::receiveMemorySize, memorySize);
    }

    void Device::executeSocketCommand(SocketHandle s, SocketCommand cmd)
    {
        writeSocketCommandRegister(s, cmd);

        while( readSocketCommandRegister(s) != SocketCommand::executed )
        {
            // Wait for completion
        }
    }

    void Device::writeSocketModeRegister(SocketHandle s, std::uint8_t value)
    {
        write(registers::socketMode(s), value);
    }

    void Device::writeSocketSourcePort(SocketHandle s, std::uint16_t value)
    {
        write(registers::socketSourcePort(s), value);
    }

    void Device::writeSocketInterruptRegister(SocketHandle s, SocketInterrupt value)
    {
        write(registers::socketInterrupt(s), value.value());
    }

    SocketInterrupt Device::readSocketInterruptRegister(SocketHandle s)
    {
        return static_cast<SocketInterrupt>(read(registers::socketInterrupt(s)));
    }

    void Device::writeSocketCommandRegister(SocketHandle s, SocketCommand value)
    {
        write(registers::socketCommand(s), static_cast<std::uint8_t>(value));
    }

    SocketCommand Device::readSocketCommandRegister(SocketHandle s)
    {
        return static_cast<SocketCommand>(read(registers::socketCommand(s)));
    }

    SocketStatus Device::readSocketStatusRegister(SocketHandle s)
    {
        return static_cast<SocketStatus>(read(registers::socketStatus(s)));
    }

    std::uint16_t Device::getTransmitFreeSize(SocketHandle s)
    {
        return readFreesize(registers::socketTransmitFreeSize(s));
    }

    std::uint16_t Device::getReceiveFreeSize(SocketHandle s)
    {
        return readFreesize(registers::socketReceiveFreeSize(s));
    }

    std::uint16_t Device::readFreesize(Register<std::uint16_t> freesizeReg)
    {
        std::uint16_t firstRead{0};
        std::uint16_t secondRead{0};

        do
        {
            firstRead = read(freesizeReg);

            if( firstRead != 0 )
            {
                secondRead = read(freesizeReg);
            }
        }
        while( secondRead != firstRead );

        return secondRead;
    }

    void Device::sendData(SocketHandle s, const gsl::span<const std::uint8_t> buffer)
    {
        constexpr std::uint16_t transmitBufferMask{0x07ff};
        const auto size = buffer.size();
        const std::uint16_t writePointer = read(registers::socketTransmitWritePointer(s));
        const std::uint16_t offset = writePointer & transmitBufferMask;
        const std::uint16_t destAddress = offset + toTransmitBufferAddress(s);

        if( isWrapAround<transmitBufferSize>(offset, size) == true )
        {
            const auto first = transmitBufferSize - offset;
            const auto border = std::next(buffer.cbegin(), first);
            write(makeRegister<gsl::span<const std::uint8_t>>(destAddress), buffer.cbegin(), border);
            write(makeRegister<gsl::span<const std::uint8_t>>(toTransmitBufferAddress(s)), border, buffer.cend());
        }
        else
        {
            write(Register<gsl::span<const std::uint8_t>>(destAddress), buffer.cbegin(), buffer.cend());
        }

        write(registers::socketTransmitWritePointer(s), static_cast<std::uint16_t>(writePointer + size));
    }

    std::uint16_t Device::receiveData(SocketHandle s, gsl::span<std::uint8_t> buffer)
    {
        constexpr std::uint16_t receiveBufferMask{0x07ff};
        const auto size = buffer.size();
        const std::uint16_t readPointer = read(registers::socketReceiveReadPointer(s));
        const std::uint16_t offset = readPointer & receiveBufferMask;
        const std::uint16_t destAddress = offset + toReceiveBufferAddress(s);
        const auto reg = makeRegister<gsl::span<std::uint8_t>>(destAddress);

        if( isWrapAround<receiveBufferSize>(offset, size) == true )
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

        write(registers::socketReceiveReadPointer(s), static_cast<std::uint16_t>(readPointer + size));

        return size;
    }

    void Device::write(std::uint16_t addr, std::uint16_t offset, std::uint8_t data)
    {
        m_writer.write(addr + offset, data);
    }

    std::uint8_t Device::read(std::uint16_t addr, std::uint16_t offset)
    {
        return m_writer.read(addr + offset);
    }

    void Device::writeModeRegister(Mode value)
    {
        write(registers::mode, static_cast<std::uint8_t>(value));
    }

    void Device::setDestAddress(SocketHandle s, NetAddress<4> addr, std::uint16_t port)
    {
        write(registers::socketDestIpAddress(s), addr.cbegin(), addr.cend());
        write(registers::socketDestPort(s), port);
    }


    void setupDevice(Device& dev, eth::NetConfig config)
    {
        dev.write(registers::sourceIpAddress, std::get<0>(config).cbegin(), std::get<0>(config).cend());
        dev.write(registers::subnetMask, std::get<1>(config).cbegin(), std::get<1>(config).cend());
        dev.write(registers::gatewayAddress, std::get<2>(config).cbegin(), std::get<2>(config).cend());
        dev.write(registers::sourceMacAddress, std::get<3>(config).cbegin(), std::get<3>(config).cend());
    }

}
}
