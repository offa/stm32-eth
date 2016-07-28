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
    W5100Device device;


    W5100Device::W5100Device() { }

    // TODO: Check object

    void W5100Device::writeSocketModeRegister(Socket s, uint8_t value)
    {
        mock("W5100Device").actualCall("writeSocketModeRegister")
                .withParameter("socket", s)
                .withParameter("value", value);
    }

    void W5100Device::writeSocketSourcePort(Socket s, uint16_t value)
    {
        mock("W5100Device").actualCall("writeSocketSourcePort")
                .withParameter("socket", s)
                .withParameter("value", value);
    }

    void W5100Device::writeSocketInterruptRegister(Socket s, uint8_t value)
    {
        mock("W5100Device").actualCall("writeSocketInterruptRegister")
                .withParameter("socket", s)
                .withParameter("value", value);
    }

    uint8_t W5100Device::readSocketInterruptRegister(Socket s)
    {
        return mock("W5100Device").actualCall("readSocketInterruptRegister")
                .withParameter("socket", s)
                .returnIntValue();
    }

    void W5100Device::executeSocketCommand(Socket s, uint8_t value)
    {
        mock("W5100Device").actualCall("executeSocketCommand")
                .withParameter("socket", s)
                .withParameter("value", value);
    }

    uint8_t W5100Device::readSocketStatusRegister(Socket s)
    {
        return mock("W5100Device").actualCall("readSocketStatusRegister")
                .withParameter("socket", s)
                .returnIntValue(); // TODO: unsigned?
    }

    uint16_t W5100Device::getTransmitFreeSize(Socket s)
    {
        return mock("W5100Device").actualCall("getTransmitFreeSize")
                .withParameter("socket", s)
                .returnIntValue();
    }

    void W5100Device::sendData(Socket s, const uint8_t* buffer, uint16_t size)
    {
        mock("W5100Device").actualCall("sendData")
                .withParameter("socket", s)
                .withMemoryBufferParameter("buffer", buffer, size)
                .withParameter("size", size);
    }
}

