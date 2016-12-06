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
#include <CppUTestExt/MockSupport.h>

namespace eth
{
    W5100Device::W5100Device(SpiWriter& writer) : m_writer(writer)
    {
    }

    void W5100Device::writeSocketModeRegister(SocketHandle s, uint8_t value)
    {
        mock("W5100Device").actualCall("writeSocketModeRegister")
                .withParameter("socket", s.get())
                .withParameter("value", value);
    }

    void W5100Device::writeSocketSourcePort(SocketHandle s, uint16_t value)
    {
        mock("W5100Device").actualCall("writeSocketSourcePort")
                .withParameter("socket", s.get())
                .withParameter("value", value);
    }

    void W5100Device::writeSocketInterruptRegister(SocketHandle s, SocketInterrupt value)
    {
        mock("W5100Device").actualCall("writeSocketInterruptRegister")
                .withParameter("socket", s.get())
                .withParameter("value", value.value());
    }

    SocketInterrupt W5100Device::readSocketInterruptRegister(SocketHandle s)
    {
        const auto value =  mock("W5100Device").actualCall("readSocketInterruptRegister")
                                .withParameter("socket", s.get())
                                .returnUnsignedIntValue();
        return SocketInterrupt(value);
    }

    void W5100Device::executeSocketCommand(SocketHandle s, SocketCommand cmd)
    {
        mock("W5100Device").actualCall("executeSocketCommand")
                .withParameter("socket", s.get())
                .withParameter("value", static_cast<uint8_t>(cmd));
    }

    SocketStatus W5100Device::readSocketStatusRegister(SocketHandle s)
    {
        return static_cast<SocketStatus>(mock("W5100Device").actualCall("readSocketStatusRegister")
                .withParameter("socket", s.get())
                .returnUnsignedIntValue());
    }

    uint16_t W5100Device::getTransmitFreeSize(SocketHandle s)
    {
        return mock("W5100Device").actualCall("getTransmitFreeSize")
                .withParameter("socket", s.get())
                .returnUnsignedIntValue();
    }

    uint16_t W5100Device::getReceiveFreeSize(SocketHandle s)
    {
        return mock("W5100Device").actualCall("getReceiveFreeSize")
                .withParameter("socket", s.get())
                .returnUnsignedIntValue();
    }

    void W5100Device::sendData(SocketHandle s, const gsl::span<const uint8_t> buffer)
    {
        mock("W5100Device").actualCall("sendData")
                .withParameter("socket", s.get())
                .withMemoryBufferParameter("buffer", buffer.data(), buffer.length())
                .withParameter("size", buffer.length());
    }

    uint16_t W5100Device::receiveData(SocketHandle s, gsl::span<uint8_t> buffer)
    {
        return mock("W5100Device").actualCall("receiveData")
                    .withParameter("socket", s.get())
                    .withOutputParameter("buffer", buffer.data())
                    .withParameter("size", buffer.length())
                    .returnUnsignedIntValue();
    }

    void W5100Device::setDestIpAddress(SocketHandle s, std::array<uint8_t, 4> addr)
    {
        mock("W5100Device").actualCall("setDestIpAddress")
                .withParameter("socket", s.get())
                .withMemoryBufferParameter("buffer", addr.data(), addr.size());
    }

    void W5100Device::setDestPort(SocketHandle s, uint16_t port)
    {
        mock("W5100Device").actualCall("setDestPort")
                .withParameter("socket", s.get())
                .withParameter("port", port);
    }

}

