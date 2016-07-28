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

#include "Socket.h"
#include "W5100Device.h"
#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>

using eth::SocketStatus;
using eth::SocketCommand;
using eth::SocketInterrupt;

namespace eth
{
    W5100Device device;

    uint8_t Spi::transfer(uint8_t data)
    {
        return mock("Spi").actualCall("transfer").withParameter("data", data).returnIntValueOrDefault(0xff);
    }

    void Spi::setSS()
    {
        // CS low
        mock("Spi").actualCall("setSS");
    }

    void Spi::resetSS()
    {
        // CS high
        mock("Spi").actualCall("resetSS");
    }

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


TEST_GROUP(SocketTest)
{
    void setup() override
    {
        mock().strictOrder();
    }

    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
    }

    void expectClose(eth::Socket socket)
    {
        mock("W5100Device").expectOneCall("executeSocketCommand")
            .withParameter("socket", socket)
            .withParameter("value", static_cast<int>(SocketCommand::close));
        mock("W5100Device").expectOneCall("writeSocketInterruptRegister")
            .withParameter("socket", socket)
            .withParameter("value", 0xff);
    }


    static constexpr eth::Socket s = 0;
    static constexpr uint8_t protocol = static_cast<uint8_t>(eth::SocketMode::tcp);
    static constexpr uint16_t port = 1234;
    static constexpr uint8_t flag = 0;
    static constexpr uint8_t statusSendOk = static_cast<uint8_t>(SocketInterrupt::sendOk);
};

TEST(SocketTest, socketReturnsErrorOnInvalidProtocol)
{
    constexpr uint8_t invalidProtocol = 0xff;
    CHECK_EQUAL(0, eth::socket(s, invalidProtocol, port, flag))
}

TEST(SocketTest, socketReturnsSuccess)
{
    mock("W5100Device").ignoreOtherCalls();
    CHECK_EQUAL(1, eth::socket(s, protocol, port, flag))
}

TEST(SocketTest, socketResetsSocketFirst)
{
    expectClose(s);
    mock("W5100Device").expectOneCall("writeSocketModeRegister").ignoreOtherParameters();
    mock("W5100Device").expectOneCall("writeSocketSourcePort").ignoreOtherParameters();
    mock("W5100Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();
    CHECK_EQUAL(1, eth::socket(s, protocol, port, flag));
}

TEST(SocketTest, socketSetsProtocol)
{
    constexpr uint8_t value = protocol | flag;
    expectClose(s);
    mock("W5100Device").expectOneCall("writeSocketModeRegister")
        .withParameter("socket", s)
        .withParameter("value", value);
    mock("W5100Device").expectOneCall("writeSocketSourcePort").ignoreOtherParameters();
    mock("W5100Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();
    CHECK_EQUAL(1, eth::socket(s, protocol, port, flag));
}

TEST(SocketTest, socketSetsFlag)
{
    constexpr uint8_t flagValue = 0x0a;
    constexpr uint8_t value = protocol | flagValue;
    expectClose(s);
    mock("W5100Device").expectOneCall("writeSocketModeRegister")
        .withParameter("socket", s)
        .withParameter("value", value);
    mock("W5100Device").expectOneCall("writeSocketSourcePort").ignoreOtherParameters();
    mock("W5100Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();
    CHECK_EQUAL(1, eth::socket(s, protocol, port, flagValue));
}

TEST(SocketTest, socketSetsPort)
{
    expectClose(s);
    mock("W5100Device").expectOneCall("writeSocketModeRegister").ignoreOtherParameters();
    mock("W5100Device").expectOneCall("writeSocketSourcePort")
        .withParameter("socket", s)
        .withParameter("value", port);
    mock("W5100Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();
    CHECK_EQUAL(1, eth::socket(s, protocol, port, flag));
}

TEST(SocketTest, socketOpensSocket)
{
    expectClose(s);
    mock("W5100Device").expectOneCall("writeSocketModeRegister").ignoreOtherParameters();
    mock("W5100Device").expectOneCall("writeSocketSourcePort").ignoreOtherParameters();
    mock("W5100Device").expectOneCall("executeSocketCommand")
        .withParameter("socket", s)
        .withParameter("value", static_cast<int>(SocketCommand::open));
    CHECK_EQUAL(1, eth::socket(s, protocol, port, flag));
}

TEST(SocketTest, close)
{
    mock("W5100Device").expectOneCall("executeSocketCommand")
        .withParameter("socket", s)
        .withParameter("value", static_cast<int>(SocketCommand::close));
    mock("W5100Device").expectOneCall("writeSocketInterruptRegister")
        .withParameter("socket", s)
        .withParameter("value", 0xff);
    eth::close(s);
}

TEST(SocketTest, listenReturnsErrorIfStatusNotInit)
{
    mock("W5100Device").expectOneCall("readSocketStatusRegister")
        .withParameter("socket", s)
        .andReturnValue(static_cast<int>(SocketStatus::closed));
    CHECK_EQUAL(0, eth::listen(s));
}

TEST(SocketTest, listenListens)
{
    mock("W5100Device").expectOneCall("readSocketStatusRegister")
        .withParameter("socket", s)
        .andReturnValue(static_cast<int>(SocketStatus::init));
    mock("W5100Device").expectOneCall("executeSocketCommand")
        .withParameter("socket", s)
        .withParameter("value", static_cast<int>(SocketCommand::listen));
    uint8_t rtn = eth::listen(s);
    CHECK_EQUAL(1, rtn);
}

TEST(SocketTest, sendReturnsBytesTransmitted)
{
    mock("W5100Device").expectOneCall("getTransmitFreeSize").ignoreOtherParameters().andReturnValue(100);
    mock("W5100Device").expectOneCall("readSocketStatusRegister").ignoreOtherParameters().andReturnValue(static_cast<int>(SocketStatus::established));
    mock("W5100Device").expectOneCall("sendData").ignoreOtherParameters();
    mock("W5100Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();
    mock("W5100Device").expectOneCall("readSocketInterruptRegister").ignoreOtherParameters().andReturnValue(statusSendOk);
    mock("W5100Device").expectOneCall("writeSocketInterruptRegister").ignoreOtherParameters();
    constexpr uint16_t len = 10;
    uint8_t buffer[len];
    CHECK_EQUAL(len, eth::send(s, buffer, len));
}

TEST(SocketTest, sendLimitsBufferSize)
{
    constexpr uint16_t maxSendSize = 2048;
    mock("W5100Device").expectOneCall("getTransmitFreeSize").ignoreOtherParameters().andReturnValue(maxSendSize);
    mock("W5100Device").expectOneCall("readSocketStatusRegister").ignoreOtherParameters().andReturnValue(static_cast<int>(SocketStatus::established));
    mock("W5100Device").expectOneCall("sendData").ignoreOtherParameters();
    mock("W5100Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();
    mock("W5100Device").expectOneCall("readSocketInterruptRegister").ignoreOtherParameters().andReturnValue(statusSendOk);
    mock("W5100Device").expectOneCall("writeSocketInterruptRegister").ignoreOtherParameters();
    constexpr uint16_t len = maxSendSize + 1;
    uint8_t buffer[len];
    CHECK_EQUAL(maxSendSize, eth::send(s, buffer, len));
}

TEST(SocketTest, sendFreesizeAndStatusFlag)
{
    // TODO: Test loop
    constexpr uint16_t size = 10;
    constexpr uint16_t freeSize = size + 2;
    constexpr uint16_t len = size;
    uint8_t buffer[len];
    mock("W5100Device").expectOneCall("getTransmitFreeSize")
        .withParameter("socket", s)
        .andReturnValue(freeSize);
    mock("W5100Device").expectOneCall("readSocketStatusRegister")
        .withParameter("socket", s)
        .andReturnValue(static_cast<int>(SocketStatus::established)); // TODO: Test also with closeWait
    mock("W5100Device").expectOneCall("sendData").ignoreOtherParameters();
    mock("W5100Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();
    mock("W5100Device").expectOneCall("readSocketInterruptRegister").ignoreOtherParameters().andReturnValue(statusSendOk);
    mock("W5100Device").expectOneCall("writeSocketInterruptRegister").ignoreOtherParameters();
    CHECK_EQUAL(size, eth::send(s, buffer, len));
}

TEST(SocketTest, sendReturnsErrorIfStatusNotEstablished)
{
    constexpr uint16_t size = 10;
    constexpr uint16_t freeSize = size + 2;
    constexpr uint16_t len = size;
    uint8_t buffer[len];
    mock("W5100Device").expectOneCall("getTransmitFreeSize")
        .withParameter("socket", s)
        .andReturnValue(freeSize);
    mock("W5100Device").expectOneCall("readSocketStatusRegister")
        .withParameter("socket", s)
        .andReturnValue(static_cast<int>(SocketStatus::init));
    mock("W5100Device").expectOneCall("sendData").ignoreOtherParameters();
    mock("W5100Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();
    mock("W5100Device").expectOneCall("readSocketInterruptRegister").ignoreOtherParameters().andReturnValue(statusSendOk);
    mock("W5100Device").expectOneCall("writeSocketInterruptRegister").ignoreOtherParameters();
    uint16_t rtn = eth::send(s, buffer, len);
    CHECK_EQUAL(0, rtn);
}

TEST(SocketTest, sendSendsDataAndCommand)
{
    constexpr uint16_t len = 10;
    uint8_t buffer[len];
    for( uint16_t i=0; i<len; ++i )
    {
        buffer[i] = i;
    }
    mock("W5100Device").expectOneCall("getTransmitFreeSize").ignoreOtherParameters().andReturnValue(len);
    mock("W5100Device").expectOneCall("readSocketStatusRegister").ignoreOtherParameters().andReturnValue(static_cast<int>(SocketStatus::established));
    mock("W5100Device").expectOneCall("sendData")
        .withParameter("socket", s)
        .withMemoryBufferParameter("buffer", buffer, len)
        .withParameter("size", len);
    mock("W5100Device").expectOneCall("executeSocketCommand")
        .withParameter("socket", s)
        .withParameter("value", static_cast<int>(SocketCommand::send));
    mock("W5100Device").expectOneCall("readSocketInterruptRegister").ignoreOtherParameters().andReturnValue(statusSendOk);
    mock("W5100Device").expectOneCall("writeSocketInterruptRegister").ignoreOtherParameters();
    uint16_t rtn = eth::send(s, buffer, len);
    CHECK_EQUAL(len, rtn);
}

TEST(SocketTest, sendWaitsForStatusFlagAfterSend)
{
    constexpr uint16_t len = 10;
    uint8_t buffer[len];
    for( uint16_t i=0; i<len; ++i )
    {
        buffer[i] = i;
    }
    mock("W5100Device").expectOneCall("getTransmitFreeSize").ignoreOtherParameters().andReturnValue(len);
    mock("W5100Device").expectOneCall("readSocketStatusRegister").withParameter("socket", s).andReturnValue(static_cast<int>(SocketStatus::established));
    mock("W5100Device").expectOneCall("sendData").ignoreOtherParameters();
    mock("W5100Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();
    constexpr uint8_t ready = statusSendOk;
    constexpr uint8_t notReady = !ready;
    mock("W5100Device").expectOneCall("readSocketInterruptRegister")
        .withParameter("socket", s)
        .andReturnValue(notReady);
    mock("W5100Device").expectOneCall("readSocketStatusRegister")
        .withParameter("socket", s)
        .andReturnValue(static_cast<int>(SocketStatus::established));
    mock("W5100Device").expectOneCall("readSocketInterruptRegister")
        .withParameter("socket", s)
        .andReturnValue(notReady);
    mock("W5100Device").expectOneCall("readSocketStatusRegister")
        .withParameter("socket", s)
        .andReturnValue(static_cast<int>(SocketStatus::established));
    mock("W5100Device").expectOneCall("readSocketInterruptRegister")
        .withParameter("socket", s)
        .andReturnValue(ready);
    mock("W5100Device").expectOneCall("writeSocketInterruptRegister").ignoreOtherParameters();

    uint16_t rtn = eth::send(s, buffer, len);
    CHECK_EQUAL(len, rtn);
}

TEST(SocketTest, sendClosesConnectionIfClosedStatus)
{
    constexpr uint16_t len = 10;
    uint8_t buffer[len];
    for( uint16_t i=0; i<len; ++i )
    {
        buffer[i] = i;
    }
    mock("W5100Device").expectOneCall("getTransmitFreeSize").ignoreOtherParameters().andReturnValue(len);
    mock("W5100Device").expectOneCall("readSocketStatusRegister").withParameter("socket", s).andReturnValue(static_cast<int>(SocketStatus::established));
    mock("W5100Device").expectOneCall("sendData").ignoreOtherParameters();
    mock("W5100Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();
    constexpr uint8_t ready = statusSendOk;
    constexpr uint8_t notReady = !ready;
    mock("W5100Device").expectOneCall("readSocketInterruptRegister")
        .withParameter("socket", s)
        .andReturnValue(notReady);
    mock("W5100Device").expectOneCall("readSocketStatusRegister")
        .withParameter("socket", s)
        .andReturnValue(static_cast<int>(SocketStatus::closed));
    expectClose(s);

    uint16_t rtn = eth::send(s, buffer, len);
    CHECK_EQUAL(0, rtn);
}

TEST(SocketTest, sendSetsOkAfterSend)
{
    constexpr uint16_t len = 10;
    uint8_t buffer[len];
    for( uint16_t i=0; i<len; ++i )
    {
        buffer[i] = i;
    }
    mock("W5100Device").expectOneCall("getTransmitFreeSize").ignoreOtherParameters().andReturnValue(len);
    mock("W5100Device").expectOneCall("readSocketStatusRegister").withParameter("socket", s).andReturnValue(static_cast<int>(SocketStatus::established));
    mock("W5100Device").expectOneCall("sendData").ignoreOtherParameters();
    mock("W5100Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();
    constexpr uint8_t ready = statusSendOk;
    mock("W5100Device").expectOneCall("readSocketInterruptRegister")
        .withParameter("socket", s)
        .andReturnValue(ready);
    mock("W5100Device").expectOneCall("writeSocketInterruptRegister")
        .withParameter("socket", s)
        .withParameter("value", statusSendOk);

    uint16_t rtn = eth::send(s, buffer, len);
    CHECK_EQUAL(len, rtn);
}

