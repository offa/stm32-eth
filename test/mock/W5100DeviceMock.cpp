/*
 * Stm32 Eth - Ethernet connectivity for Stm32
 * Copyright (C) 2016-2019  offa
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

namespace eth::w5100
{

    Device::Device(spi::SpiWriter& writer)
        : spiWriter(writer)
    {
    }

    void Device::writeSocketModeRegister(SocketHandle s, std::uint8_t value)
    {
        mock("Device").actualCall("writeSocketModeRegister").withParameter("socket", s.value()).withParameter("value", value);
    }

    void Device::writeSocketSourcePort(SocketHandle s, std::uint16_t value)
    {
        mock("Device").actualCall("writeSocketSourcePort").withParameter("socket", s.value()).withParameter("value", value);
    }

    void Device::writeSocketInterruptRegister(SocketHandle s, SocketInterrupt value)
    {
        mock("Device")
            .actualCall("writeSocketInterruptRegister")
            .withParameter("socket", s.value())
            .withParameter("value", value.value());
    }

    SocketInterrupt Device::readSocketInterruptRegister(SocketHandle s)
    {
        const auto value =
            mock("Device").actualCall("readSocketInterruptRegister").withParameter("socket", s.value()).returnUnsignedIntValue();
        return SocketInterrupt(value);
    }

    void Device::executeSocketCommand(SocketHandle s, SocketCommand cmd)
    {
        mock("Device")
            .actualCall("executeSocketCommand")
            .withParameter("socket", s.value())
            .withParameter("value", static_cast<std::uint8_t>(cmd));
    }

    SocketStatus Device::readSocketStatusRegister(SocketHandle s)
    {
        return static_cast<SocketStatus>(
            mock("Device").actualCall("readSocketStatusRegister").withParameter("socket", s.value()).returnUnsignedIntValue());
    }

    std::uint16_t Device::getTransmitFreeSize(SocketHandle s)
    {
        return mock("Device").actualCall("getTransmitFreeSize").withParameter("socket", s.value()).returnUnsignedIntValue();
    }

    std::uint16_t Device::getReceiveFreeSize(SocketHandle s)
    {
        return mock("Device").actualCall("getReceiveFreeSize").withParameter("socket", s.value()).returnUnsignedIntValue();
    }

    void Device::sendData(SocketHandle s, const gsl::span<const std::uint8_t> buffer)
    {
        mock("Device")
            .actualCall("sendData")
            .withParameter("socket", s.value())
            .withMemoryBufferParameter("buffer", buffer.data(), buffer.size())
            .withParameter("size", buffer.size());
    }

    std::uint16_t Device::receiveData(SocketHandle s, gsl::span<std::uint8_t> buffer)
    {
        return mock("Device")
            .actualCall("receiveData")
            .withParameter("socket", s.value())
            .withOutputParameter("buffer", buffer.data())
            .withParameter("size", buffer.size())
            .returnUnsignedIntValue();
    }

    void Device::setDestAddress(SocketHandle s, NetAddress<4> addr, std::uint16_t port)
    {
        mock("Device")
            .actualCall("setDestAddress")
            .withParameter("socket", s.value())
            .withMemoryBufferParameter("buffer", addr.data(), addr.size())
            .withParameter("port", port);
    }

}
