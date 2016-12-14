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

#include "w5100/Device.h"
#include <CppUTestExt/MockSupport.h>

namespace eth
{
namespace w5100
{

    Device::Device(SpiWriter& writer) : m_writer(writer)
    {
    }

    void Device::writeSocketModeRegister(SocketHandle s, uint8_t value)
    {
        mock("Device").actualCall("writeSocketModeRegister")
                .withParameter("socket", s.value())
                .withParameter("value", value);
    }

    void Device::writeSocketSourcePort(SocketHandle s, uint16_t value)
    {
        mock("Device").actualCall("writeSocketSourcePort")
                .withParameter("socket", s.value())
                .withParameter("value", value);
    }

    void Device::writeSocketInterruptRegister(SocketHandle s, SocketInterrupt value)
    {
        mock("Device").actualCall("writeSocketInterruptRegister")
                .withParameter("socket", s.value())
                .withParameter("value", value.value());
    }

    SocketInterrupt Device::readSocketInterruptRegister(SocketHandle s)
    {
        const auto value =  mock("Device").actualCall("readSocketInterruptRegister")
                                .withParameter("socket", s.value())
                                .returnUnsignedIntValue();
        return SocketInterrupt(value);
    }

    void Device::executeSocketCommand(SocketHandle s, SocketCommand cmd)
    {
        mock("Device").actualCall("executeSocketCommand")
                .withParameter("socket", s.value())
                .withParameter("value", static_cast<uint8_t>(cmd));
    }

    SocketStatus Device::readSocketStatusRegister(SocketHandle s)
    {
        return static_cast<SocketStatus>(mock("Device").actualCall("readSocketStatusRegister")
                .withParameter("socket", s.value())
                .returnUnsignedIntValue());
    }

    uint16_t Device::getTransmitFreeSize(SocketHandle s)
    {
        return mock("Device").actualCall("getTransmitFreeSize")
                .withParameter("socket", s.value())
                .returnUnsignedIntValue();
    }

    uint16_t Device::getReceiveFreeSize(SocketHandle s)
    {
        return mock("Device").actualCall("getReceiveFreeSize")
                .withParameter("socket", s.value())
                .returnUnsignedIntValue();
    }

    void Device::sendData(SocketHandle s, const gsl::span<const uint8_t> buffer)
    {
        mock("Device").actualCall("sendData")
                .withParameter("socket", s.value())
                .withMemoryBufferParameter("buffer", buffer.data(), buffer.length())
                .withParameter("size", buffer.length());
    }

    uint16_t Device::receiveData(SocketHandle s, gsl::span<uint8_t> buffer)
    {
        return mock("Device").actualCall("receiveData")
                    .withParameter("socket", s.value())
                    .withOutputParameter("buffer", buffer.data())
                    .withParameter("size", buffer.length())
                    .returnUnsignedIntValue();
    }

    void Device::setDestIpAddress(SocketHandle s, std::array<uint8_t, 4> addr)
    {
        mock("Device").actualCall("setDestIpAddress")
                .withParameter("socket", s.value())
                .withMemoryBufferParameter("buffer", addr.data(), addr.size());
    }

    void Device::setDestPort(SocketHandle s, uint16_t port)
    {
        mock("Device").actualCall("setDestPort")
                .withParameter("socket", s.value())
                .withParameter("port", port);
    }

}
}
