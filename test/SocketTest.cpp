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
#include "SocketMode.h"
#include "SocketStatus.h"
#include "SocketCommand.h"
#include "SocketInterrupt.h"
#include "W5100Device.h"
#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>

using eth::SocketStatus;
using eth::SocketCommand;
using eth::SocketInterrupt;
using eth::Socket;


TEST_GROUP(SocketTest)
{
    void setup() override
    {
        constexpr auto handle = socketHandle;
        socket = std::make_unique<Socket>(handle);
        mock().strictOrder();
    }

    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
    }

    void expectClose(eth::SocketHandle socketHandle)
    {
        mock("W5100Device").expectOneCall("executeSocketCommand")
            .withParameter("socket", socketHandle)
            .withParameter("value", static_cast<int>(SocketCommand::close));
        mock("W5100Device").expectOneCall("writeSocketInterruptRegister")
            .withParameter("socket", socketHandle)
            .withParameter("value", 0xff);
    }


    std::unique_ptr<Socket> socket;
    static constexpr eth::SocketHandle socketHandle = 0;
    static constexpr uint8_t protocol = static_cast<uint8_t>(eth::SocketMode::tcp);
    static constexpr uint16_t port = 1234;
    static constexpr uint8_t flag = 0;
    static constexpr uint8_t statusSendOk = static_cast<uint8_t>(SocketInterrupt::sendOk);
};

TEST(SocketTest, openReturnsErrorOnInvalidProtocol)
{
    constexpr uint8_t invalidProtocol = 0xff;
    CHECK_FALSE(socket->open(invalidProtocol, port, flag));
}

TEST(SocketTest, openReturnsSuccess)
{
    mock("W5100Device").ignoreOtherCalls();
    CHECK_TRUE(socket->open(protocol, port, flag));
}

TEST(SocketTest, openResetsSocketFirst)
{
    expectClose(socketHandle);
    mock("W5100Device").expectOneCall("writeSocketModeRegister").ignoreOtherParameters();
    mock("W5100Device").expectOneCall("writeSocketSourcePort").ignoreOtherParameters();
    mock("W5100Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();
    CHECK_TRUE(socket->open(protocol, port, flag));
}

TEST(SocketTest, openSetsProtocol)
{
    constexpr uint8_t value = protocol | flag;
    expectClose(socketHandle);
    mock("W5100Device").expectOneCall("writeSocketModeRegister")
        .withParameter("socket", socketHandle)
        .withParameter("value", value);
    mock("W5100Device").expectOneCall("writeSocketSourcePort").ignoreOtherParameters();
    mock("W5100Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();
    CHECK_TRUE(socket->open(protocol, port, flag));
}

TEST(SocketTest, openSetsFlag)
{
    constexpr uint8_t flagValue = 0x0a;
    constexpr uint8_t value = protocol | flagValue;
    expectClose(socketHandle);
    mock("W5100Device").expectOneCall("writeSocketModeRegister")
        .withParameter("socket", socketHandle)
        .withParameter("value", value);
    mock("W5100Device").expectOneCall("writeSocketSourcePort").ignoreOtherParameters();
    mock("W5100Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();
    CHECK_TRUE(socket->open(protocol, port, flagValue));
}

TEST(SocketTest, openSetsPort)
{
    expectClose(socketHandle);
    mock("W5100Device").expectOneCall("writeSocketModeRegister").ignoreOtherParameters();
    mock("W5100Device").expectOneCall("writeSocketSourcePort")
        .withParameter("socket", socketHandle)
        .withParameter("value", port);
    mock("W5100Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();
    CHECK_TRUE(socket->open(protocol, port, flag));
}

TEST(SocketTest, openOpensSocket)
{
    expectClose(socketHandle);
    mock("W5100Device").expectOneCall("writeSocketModeRegister").ignoreOtherParameters();
    mock("W5100Device").expectOneCall("writeSocketSourcePort").ignoreOtherParameters();
    mock("W5100Device").expectOneCall("executeSocketCommand")
        .withParameter("socket", socketHandle)
        .withParameter("value", static_cast<int>(SocketCommand::open));
    CHECK_TRUE(socket->open(protocol, port, flag));
}

TEST(SocketTest, close)
{
    mock("W5100Device").expectOneCall("executeSocketCommand")
        .withParameter("socket", socketHandle)
        .withParameter("value", static_cast<int>(SocketCommand::close));
    mock("W5100Device").expectOneCall("writeSocketInterruptRegister")
        .withParameter("socket", socketHandle)
        .withParameter("value", 0xff);
    socket->close();
}

TEST(SocketTest, listenReturnsErrorIfStatusNotInit)
{
    mock("W5100Device").expectOneCall("readSocketStatusRegister")
        .withParameter("socket", socketHandle)
        .andReturnValue(static_cast<int>(SocketStatus::closed));
    CHECK_FALSE(socket->listen());
}

TEST(SocketTest, listenListens)
{
    mock("W5100Device").expectOneCall("readSocketStatusRegister")
        .withParameter("socket", socketHandle)
        .andReturnValue(static_cast<int>(SocketStatus::init));
    mock("W5100Device").expectOneCall("executeSocketCommand")
        .withParameter("socket", socketHandle)
        .withParameter("value", static_cast<int>(SocketCommand::listen));
    CHECK_TRUE(socket->listen());
}

TEST(SocketTest, sendReturnsBytesTransmitted)
{
    mock("W5100Device").expectOneCall("getTransmitFreeSize").ignoreOtherParameters().andReturnValue(100);
    mock("W5100Device").expectOneCall("readSocketStatusRegister").ignoreOtherParameters().andReturnValue(static_cast<int>(SocketStatus::established));
    mock("W5100Device").expectOneCall("sendData").ignoreOtherParameters();
    mock("W5100Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();
    mock("W5100Device").expectOneCall("readSocketInterruptRegister").ignoreOtherParameters().andReturnValue(statusSendOk);
    mock("W5100Device").expectOneCall("writeSocketInterruptRegister").ignoreOtherParameters();
    constexpr uint16_t length = 10;
    uint8_t buffer[length];
    CHECK_EQUAL(length, socket->send(buffer, length));
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
    constexpr uint16_t length = maxSendSize + 1;
    uint8_t buffer[length];
    CHECK_EQUAL(maxSendSize, socket->send(buffer, length));
}

TEST(SocketTest, sendFreesizeAndStatusFlag)
{
    // TODO: Test loop
    constexpr uint16_t size = 10;
    constexpr uint16_t freeSize = size + 2;
    constexpr uint16_t length = size;
    uint8_t buffer[length];
    mock("W5100Device").expectOneCall("getTransmitFreeSize")
        .withParameter("socket", socketHandle)
        .andReturnValue(freeSize);
    mock("W5100Device").expectOneCall("readSocketStatusRegister")
        .withParameter("socket", socketHandle)
        .andReturnValue(static_cast<int>(SocketStatus::established)); // TODO: Test also with closeWait
    mock("W5100Device").expectOneCall("sendData").ignoreOtherParameters();
    mock("W5100Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();
    mock("W5100Device").expectOneCall("readSocketInterruptRegister").ignoreOtherParameters().andReturnValue(statusSendOk);
    mock("W5100Device").expectOneCall("writeSocketInterruptRegister").ignoreOtherParameters();
    CHECK_EQUAL(size, socket->send(buffer, length));
}

TEST(SocketTest, sendReturnsErrorIfStatusNotEstablished)
{
    constexpr uint16_t size = 10;
    constexpr uint16_t freeSize = size + 2;
    constexpr uint16_t length = size;
    uint8_t buffer[length];
    mock("W5100Device").expectOneCall("getTransmitFreeSize")
        .withParameter("socket", socketHandle)
        .andReturnValue(freeSize);
    mock("W5100Device").expectOneCall("readSocketStatusRegister")
        .withParameter("socket", socketHandle)
        .andReturnValue(static_cast<int>(SocketStatus::init));
    mock("W5100Device").expectOneCall("sendData").ignoreOtherParameters();
    mock("W5100Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();
    mock("W5100Device").expectOneCall("readSocketInterruptRegister").ignoreOtherParameters().andReturnValue(statusSendOk);
    mock("W5100Device").expectOneCall("writeSocketInterruptRegister").ignoreOtherParameters();
    CHECK_EQUAL(0, socket->send(buffer, length));
}

TEST(SocketTest, sendSendsDataAndCommand)
{
    constexpr uint16_t length = 10;
    uint8_t buffer[length];
    for( uint16_t i=0; i<length; ++i )
    {
        buffer[i] = i;
    }
    mock("W5100Device").expectOneCall("getTransmitFreeSize").ignoreOtherParameters().andReturnValue(length);
    mock("W5100Device").expectOneCall("readSocketStatusRegister").ignoreOtherParameters().andReturnValue(static_cast<int>(SocketStatus::established));
    mock("W5100Device").expectOneCall("sendData")
        .withParameter("socket", socketHandle)
        .withMemoryBufferParameter("buffer", buffer, length)
        .withParameter("size", length);
    mock("W5100Device").expectOneCall("executeSocketCommand")
        .withParameter("socket", socketHandle)
        .withParameter("value", static_cast<int>(SocketCommand::send));
    mock("W5100Device").expectOneCall("readSocketInterruptRegister").ignoreOtherParameters().andReturnValue(statusSendOk);
    mock("W5100Device").expectOneCall("writeSocketInterruptRegister").ignoreOtherParameters();
    CHECK_EQUAL(length, socket->send(buffer, length));
}

TEST(SocketTest, sendWaitsForStatusFlagAfterSend)
{
    constexpr uint16_t length = 10;
    uint8_t buffer[length];
    for( uint16_t i=0; i<length; ++i )
    {
        buffer[i] = i;
    }
    mock("W5100Device").expectOneCall("getTransmitFreeSize").ignoreOtherParameters().andReturnValue(length);
    mock("W5100Device").expectOneCall("readSocketStatusRegister").withParameter("socket", socketHandle).andReturnValue(static_cast<int>(SocketStatus::established));
    mock("W5100Device").expectOneCall("sendData").ignoreOtherParameters();
    mock("W5100Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();
    constexpr uint8_t ready = statusSendOk;
    constexpr uint8_t notReady = !ready;
    mock("W5100Device").expectOneCall("readSocketInterruptRegister")
        .withParameter("socket", socketHandle)
        .andReturnValue(notReady);
    mock("W5100Device").expectOneCall("readSocketStatusRegister")
        .withParameter("socket", socketHandle)
        .andReturnValue(static_cast<int>(SocketStatus::established));
    mock("W5100Device").expectOneCall("readSocketInterruptRegister")
        .withParameter("socket", socketHandle)
        .andReturnValue(notReady);
    mock("W5100Device").expectOneCall("readSocketStatusRegister")
        .withParameter("socket", socketHandle)
        .andReturnValue(static_cast<int>(SocketStatus::established));
    mock("W5100Device").expectOneCall("readSocketInterruptRegister")
        .withParameter("socket", socketHandle)
        .andReturnValue(ready);
    mock("W5100Device").expectOneCall("writeSocketInterruptRegister").ignoreOtherParameters();

    CHECK_EQUAL(length, socket->send(buffer, length));
}

TEST(SocketTest, sendClosesConnectionIfClosedStatus)
{
    constexpr uint16_t length = 10;
    uint8_t buffer[length];
    for( uint16_t i=0; i<length; ++i )
    {
        buffer[i] = i;
    }
    mock("W5100Device").expectOneCall("getTransmitFreeSize").ignoreOtherParameters().andReturnValue(length);
    mock("W5100Device").expectOneCall("readSocketStatusRegister").withParameter("socket", socketHandle).andReturnValue(static_cast<int>(SocketStatus::established));
    mock("W5100Device").expectOneCall("sendData").ignoreOtherParameters();
    mock("W5100Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();
    constexpr uint8_t ready = statusSendOk;
    constexpr uint8_t notReady = !ready;
    mock("W5100Device").expectOneCall("readSocketInterruptRegister")
        .withParameter("socket", socketHandle)
        .andReturnValue(notReady);
    mock("W5100Device").expectOneCall("readSocketStatusRegister")
        .withParameter("socket", socketHandle)
        .andReturnValue(static_cast<int>(SocketStatus::closed));
    expectClose(socketHandle);

    CHECK_EQUAL(0, socket->send(buffer, length));
}

TEST(SocketTest, sendSetsOkAfterSend)
{
    constexpr uint16_t length = 10;
    uint8_t buffer[length];
    for( uint16_t i=0; i<length; ++i )
    {
        buffer[i] = i;
    }
    mock("W5100Device").expectOneCall("getTransmitFreeSize").ignoreOtherParameters().andReturnValue(length);
    mock("W5100Device").expectOneCall("readSocketStatusRegister").withParameter("socket", socketHandle).andReturnValue(static_cast<int>(SocketStatus::established));
    mock("W5100Device").expectOneCall("sendData").ignoreOtherParameters();
    mock("W5100Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();
    constexpr uint8_t ready = statusSendOk;
    mock("W5100Device").expectOneCall("readSocketInterruptRegister")
        .withParameter("socket", socketHandle)
        .andReturnValue(ready);
    mock("W5100Device").expectOneCall("writeSocketInterruptRegister")
        .withParameter("socket", socketHandle)
        .withParameter("value", statusSendOk);

    CHECK_EQUAL(length, socket->send(buffer, length));
}

