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
#include <vector>
#include <numeric>

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

    void expectClose(eth::SocketHandle handle)
    {
        mock("W5100Device").expectOneCall("executeSocketCommand")
            .withParameter("socket", handle)
            .withParameter("value", static_cast<int>(SocketCommand::close));
        mock("W5100Device").expectOneCall("writeSocketInterruptRegister")
            .withParameter("socket", handle)
            .withParameter("value", 0xff);
    }

    std::vector<uint8_t> createBuffer(size_t size) const
    {
        std::vector<uint8_t> buffer(size);
        std::iota(buffer.begin(), buffer.end(), 0);
        return buffer;
    }


    std::unique_ptr<Socket> socket;
    static constexpr eth::SocketHandle socketHandle = 0;
    static constexpr uint8_t protocol = static_cast<uint8_t>(eth::SocketMode::tcp);
    static constexpr uint16_t port = 1234;
    static constexpr uint8_t flag = 0;
    static constexpr uint8_t statusSendOk = static_cast<uint8_t>(SocketInterrupt::sendOk);
    static constexpr size_t defaultSize = 10;
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

    auto buffer = createBuffer(defaultSize);
    CHECK_EQUAL(buffer.size(), socket->send(buffer.data(), buffer.size()));
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

    auto buffer = createBuffer(maxSendSize + 1);
    CHECK_EQUAL(maxSendSize, socket->send(buffer.data(), buffer.size()));
}

TEST(SocketTest, sendChecksFreesizeAndStatusFlagIfEstablished)
{
    constexpr uint16_t freeSize = defaultSize + 2;
    mock("W5100Device").expectOneCall("getTransmitFreeSize")
        .withParameter("socket", socketHandle)
        .andReturnValue(freeSize);
    mock("W5100Device").expectOneCall("readSocketStatusRegister")
        .withParameter("socket", socketHandle)
        .andReturnValue(static_cast<int>(SocketStatus::established));
    mock("W5100Device").expectOneCall("sendData").ignoreOtherParameters();
    mock("W5100Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();
    mock("W5100Device").expectOneCall("readSocketInterruptRegister").ignoreOtherParameters().andReturnValue(statusSendOk);
    mock("W5100Device").expectOneCall("writeSocketInterruptRegister").ignoreOtherParameters();

    auto buffer = createBuffer(defaultSize);
    CHECK_EQUAL(buffer.size(), socket->send(buffer.data(), buffer.size()));
}

TEST(SocketTest, sendChecksFreesizeAndStatusFlagIfCloseWait)
{
    constexpr uint16_t freeSize = defaultSize + 2;
    mock("W5100Device").expectOneCall("getTransmitFreeSize")
        .withParameter("socket", socketHandle)
        .andReturnValue(freeSize);
    mock("W5100Device").expectOneCall("readSocketStatusRegister")
        .withParameter("socket", socketHandle)
        .andReturnValue(static_cast<int>(SocketStatus::closeWait));
    mock("W5100Device").expectOneCall("sendData").ignoreOtherParameters();
    mock("W5100Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();
    mock("W5100Device").expectOneCall("readSocketInterruptRegister").ignoreOtherParameters().andReturnValue(statusSendOk);
    mock("W5100Device").expectOneCall("writeSocketInterruptRegister").ignoreOtherParameters();

    auto buffer = createBuffer(defaultSize);
    CHECK_EQUAL(buffer.size(), socket->send(buffer.data(), buffer.size()));
}

TEST(SocketTest, sendChecksFreesizeAndStatusFlagNotEnoughFreeMemory)
{
    constexpr uint16_t freeSize = defaultSize;
    mock("W5100Device").expectOneCall("getTransmitFreeSize")
        .withParameter("socket", socketHandle)
        .andReturnValue(freeSize - 2);
    mock("W5100Device").expectOneCall("readSocketStatusRegister")
        .withParameter("socket", socketHandle)
        .andReturnValue(static_cast<int>(SocketStatus::established));
    mock("W5100Device").expectOneCall("getTransmitFreeSize")
        .withParameter("socket", socketHandle)
        .andReturnValue(freeSize);
    mock("W5100Device").expectOneCall("readSocketStatusRegister")
        .withParameter("socket", socketHandle)
        .andReturnValue(static_cast<int>(SocketStatus::established));
    mock("W5100Device").expectOneCall("sendData").ignoreOtherParameters();
    mock("W5100Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();
    mock("W5100Device").expectOneCall("readSocketInterruptRegister").ignoreOtherParameters().andReturnValue(statusSendOk);
    mock("W5100Device").expectOneCall("writeSocketInterruptRegister").ignoreOtherParameters();

    auto buffer = createBuffer(defaultSize);
    CHECK_EQUAL(buffer.size(), socket->send(buffer.data(), buffer.size()));
}

TEST(SocketTest, sendReturnsErrorIfStatusNotEstablished)
{
    constexpr uint16_t freeSize = defaultSize + 2;
    mock("W5100Device").expectOneCall("getTransmitFreeSize")
        .withParameter("socket", socketHandle)
        .andReturnValue(freeSize);
    mock("W5100Device").expectOneCall("readSocketStatusRegister")
        .withParameter("socket", socketHandle)
        .andReturnValue(static_cast<int>(SocketStatus::init));

    auto buffer = createBuffer(defaultSize);
    CHECK_EQUAL(0, socket->send(buffer.data(), buffer.size()));
}

TEST(SocketTest, sendSendsDataAndCommand)
{
    auto buffer = createBuffer(defaultSize);
    mock("W5100Device").expectOneCall("getTransmitFreeSize").ignoreOtherParameters().andReturnValue(static_cast<int>(buffer.size()));
    mock("W5100Device").expectOneCall("readSocketStatusRegister").ignoreOtherParameters().andReturnValue(static_cast<int>(SocketStatus::established));
    mock("W5100Device").expectOneCall("sendData")
        .withParameter("socket", socketHandle)
        .withMemoryBufferParameter("buffer", buffer.data(), buffer.size())
        .withParameter("size", buffer.size());
    mock("W5100Device").expectOneCall("executeSocketCommand")
        .withParameter("socket", socketHandle)
        .withParameter("value", static_cast<int>(SocketCommand::send));
    mock("W5100Device").expectOneCall("readSocketInterruptRegister").ignoreOtherParameters().andReturnValue(statusSendOk);
    mock("W5100Device").expectOneCall("writeSocketInterruptRegister").ignoreOtherParameters();

    CHECK_EQUAL(buffer.size(), socket->send(buffer.data(), buffer.size()));
}

TEST(SocketTest, sendWaitsForStatusFlagAfterSend)
{
    auto buffer = createBuffer(defaultSize);
    mock("W5100Device").expectOneCall("getTransmitFreeSize").ignoreOtherParameters().andReturnValue(static_cast<int>(buffer.size()));
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

    CHECK_EQUAL(buffer.size(), socket->send(buffer.data(), buffer.size()));
}

TEST(SocketTest, sendClosesConnectionIfClosedStatus)
{
    auto buffer = createBuffer(defaultSize);
    mock("W5100Device").expectOneCall("getTransmitFreeSize").ignoreOtherParameters().andReturnValue(static_cast<int>(buffer.size()));
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

    CHECK_EQUAL(0, socket->send(buffer.data(), buffer.size()));
}

TEST(SocketTest, sendSetsOkAfterSend)
{
    auto buffer = createBuffer(defaultSize);
    mock("W5100Device").expectOneCall("getTransmitFreeSize").ignoreOtherParameters().andReturnValue(static_cast<int>(buffer.size()));
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

    CHECK_EQUAL(buffer.size(), socket->send(buffer.data(), buffer.size()));
}

