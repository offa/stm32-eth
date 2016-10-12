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
#include "SocketStatus.h"
#include "SocketCommand.h"
#include "SocketInterrupt.h"
#include "W5100Device.h"
#include "Spi.h"
#include "TestHelper.h"
#include <vector>
#include <numeric>
#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>

using eth::SocketStatus;
using eth::SocketCommand;
using eth::SocketInterrupt;
using eth::Socket;
using eth::Protocol;
using eth::W5100Device;
using eth::Spi;

TEST_GROUP(SocketTest)
{
    void setup() override
    {
        constexpr auto handle = socketHandle;
        socket = std::make_unique<Socket>(handle, device);
        mock().strictOrder();
    }

    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
    }

    void expectClose(eth::SocketHandle handle)
    {
        deviceMock.expectOneCall("executeSocketCommand")
            .withParameter("socket", handle)
            .withParameter("value", static_cast<int>(SocketCommand::close));
        deviceMock.expectOneCall("writeSocketInterruptRegister")
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
    Spi spi;
    W5100Device device{spi};
    MockSupport& deviceMock = mock("W5100Device");
    MockSupport& platformMock = mock("platform::stm32");
    static constexpr eth::SocketHandle socketHandle = 0;
    static constexpr uint16_t port = 1234;
    static constexpr Protocol protocol = Protocol::tcp;
    static constexpr uint8_t flag = 0;
    static constexpr uint8_t statusSendOk = static_cast<uint8_t>(SocketInterrupt::send);
    static constexpr uint16_t defaultSize = 10;
};

TEST(SocketTest, openReturnsErrorOnUnsupportedProtocol)
{
    constexpr Protocol invalidProtocol = Protocol::pppoe;
    CHECK_FALSE(socket->open(invalidProtocol, port, flag));
}

TEST(SocketTest, openReturnsSuccess)
{
    deviceMock.ignoreOtherCalls();
    CHECK_TRUE(socket->open(protocol, port, flag));
}

TEST(SocketTest, openResetsSocketFirst)
{
    expectClose(socketHandle);
    deviceMock.expectOneCall("writeSocketModeRegister").ignoreOtherParameters();
    deviceMock.expectOneCall("writeSocketSourcePort").ignoreOtherParameters();
    deviceMock.expectOneCall("executeSocketCommand").ignoreOtherParameters();
    CHECK_TRUE(socket->open(protocol, port, flag));
}

TEST(SocketTest, openSetsProtocol)
{
    constexpr uint8_t value = static_cast<uint8_t>(protocol) | flag;
    expectClose(socketHandle);
    deviceMock.expectOneCall("writeSocketModeRegister")
        .withParameter("socket", socketHandle)
        .withParameter("value", value);
    deviceMock.expectOneCall("writeSocketSourcePort").ignoreOtherParameters();
    deviceMock.expectOneCall("executeSocketCommand").ignoreOtherParameters();
    CHECK_TRUE(socket->open(protocol, port, flag));
}

TEST(SocketTest, openSetsFlag)
{
    constexpr uint8_t flagValue = 0x0a;
    constexpr uint8_t value = static_cast<uint8_t>(protocol) | flagValue;
    expectClose(socketHandle);
    deviceMock.expectOneCall("writeSocketModeRegister")
        .withParameter("socket", socketHandle)
        .withParameter("value", value);
    deviceMock.expectOneCall("writeSocketSourcePort").ignoreOtherParameters();
    deviceMock.expectOneCall("executeSocketCommand").ignoreOtherParameters();
    CHECK_TRUE(socket->open(protocol, port, flagValue));
}

TEST(SocketTest, openSetsPort)
{
    expectClose(socketHandle);
    deviceMock.expectOneCall("writeSocketModeRegister").ignoreOtherParameters();
    deviceMock.expectOneCall("writeSocketSourcePort")
        .withParameter("socket", socketHandle)
        .withParameter("value", port);
    deviceMock.expectOneCall("executeSocketCommand").ignoreOtherParameters();
    CHECK_TRUE(socket->open(protocol, port, flag));
}

TEST(SocketTest, openOpensSocket)
{
    expectClose(socketHandle);
    deviceMock.expectOneCall("writeSocketModeRegister").ignoreOtherParameters();
    deviceMock.expectOneCall("writeSocketSourcePort").ignoreOtherParameters();
    deviceMock.expectOneCall("executeSocketCommand")
        .withParameter("socket", socketHandle)
        .withParameter("value", static_cast<int>(SocketCommand::open));
    CHECK_TRUE(socket->open(protocol, port, flag));
}

TEST(SocketTest, close)
{
    deviceMock.expectOneCall("executeSocketCommand")
        .withParameter("socket", socketHandle)
        .withParameter("value", static_cast<int>(SocketCommand::close));
    deviceMock.expectOneCall("writeSocketInterruptRegister")
        .withParameter("socket", socketHandle)
        .withParameter("value", 0xff);
    socket->close();
}

TEST(SocketTest, listenReturnsErrorIfStatusNotInit)
{
    deviceMock.expectOneCall("readSocketStatusRegister")
        .withParameter("socket", socketHandle)
        .andReturnValue(static_cast<int>(SocketStatus::closed));
    CHECK_FALSE(socket->listen());
}

TEST(SocketTest, listenListens)
{
    deviceMock.expectOneCall("readSocketStatusRegister")
        .withParameter("socket", socketHandle)
        .andReturnValue(static_cast<int>(SocketStatus::init));
    deviceMock.expectOneCall("executeSocketCommand")
        .withParameter("socket", socketHandle)
        .withParameter("value", static_cast<int>(SocketCommand::listen));
    CHECK_TRUE(socket->listen());
}

TEST(SocketTest, acceptWaitsBetweenStatusCheck)
{
    constexpr uint32_t waitTime = 100;
    deviceMock.expectOneCall("readSocketStatusRegister")
        .ignoreOtherParameters()
        .andReturnValue(static_cast<int>(SocketStatus::listen));
    deviceMock.expectOneCall("readSocketStatusRegister")
        .ignoreOtherParameters()
        .andReturnValue(static_cast<int>(SocketStatus::established));
    platformMock.expectOneCall("wait").withParameter("timeMs", waitTime);
    socket->accept();
}

TEST(SocketTest, acceptWaitsForConnection)
{
    constexpr int n = 3;
    platformMock.expectNCalls(n, "wait").ignoreOtherParameters();
    deviceMock.expectNCalls(n, "readSocketStatusRegister")
        .ignoreOtherParameters()
        .andReturnValue(static_cast<int>(SocketStatus::listen));
    deviceMock.expectOneCall("readSocketStatusRegister")
        .ignoreOtherParameters()
        .andReturnValue(static_cast<int>(SocketStatus::established));
    socket->accept();
}

TEST(SocketTest, sendReturnsBytesTransmitted)
{
    deviceMock.expectOneCall("getTransmitFreeSize").ignoreOtherParameters().andReturnValue(100);
    deviceMock.expectOneCall("readSocketStatusRegister").ignoreOtherParameters().andReturnValue(static_cast<int>(SocketStatus::established));
    deviceMock.expectOneCall("sendData").ignoreOtherParameters();
    deviceMock.expectOneCall("executeSocketCommand").ignoreOtherParameters();
    deviceMock.expectOneCall("readSocketInterruptRegister").ignoreOtherParameters().andReturnValue(statusSendOk);
    deviceMock.expectOneCall("writeSocketInterruptRegister").ignoreOtherParameters();

    auto buffer = createBuffer(defaultSize);
    const auto result = socket->send(buffer);
    CHECK_EQUAL(buffer.size(), result);
}

TEST(SocketTest, sendLimitsBufferSize)
{
    constexpr uint16_t maxSendSize = 2048;
    deviceMock.expectOneCall("getTransmitFreeSize").ignoreOtherParameters().andReturnValue(maxSendSize);
    deviceMock.expectOneCall("readSocketStatusRegister").ignoreOtherParameters().andReturnValue(static_cast<int>(SocketStatus::established));
    deviceMock.expectOneCall("sendData").ignoreOtherParameters();
    deviceMock.expectOneCall("executeSocketCommand").ignoreOtherParameters();
    deviceMock.expectOneCall("readSocketInterruptRegister").ignoreOtherParameters().andReturnValue(statusSendOk);
    deviceMock.expectOneCall("writeSocketInterruptRegister").ignoreOtherParameters();

    auto buffer = createBuffer(maxSendSize + 1);
    const auto result = socket->send(buffer);
    CHECK_EQUAL(maxSendSize, result);
}

TEST(SocketTest, sendChecksFreesizeAndStatusFlagIfEstablished)
{
    constexpr uint16_t freeSize = defaultSize + 2;
    deviceMock.expectOneCall("getTransmitFreeSize")
        .withParameter("socket", socketHandle)
        .andReturnValue(freeSize);
    deviceMock.expectOneCall("readSocketStatusRegister")
        .withParameter("socket", socketHandle)
        .andReturnValue(static_cast<int>(SocketStatus::established));
    deviceMock.expectOneCall("sendData").ignoreOtherParameters();
    deviceMock.expectOneCall("executeSocketCommand").ignoreOtherParameters();
    deviceMock.expectOneCall("readSocketInterruptRegister").ignoreOtherParameters().andReturnValue(statusSendOk);
    deviceMock.expectOneCall("writeSocketInterruptRegister").ignoreOtherParameters();

    auto buffer = createBuffer(defaultSize);
    const auto result = socket->send(buffer);
    CHECK_EQUAL(buffer.size(), result);
}

TEST(SocketTest, sendChecksFreesizeAndStatusFlagIfCloseWait)
{
    constexpr uint16_t freeSize = defaultSize + 2;
    deviceMock.expectOneCall("getTransmitFreeSize")
        .withParameter("socket", socketHandle)
        .andReturnValue(freeSize);
    deviceMock.expectOneCall("readSocketStatusRegister")
        .withParameter("socket", socketHandle)
        .andReturnValue(static_cast<int>(SocketStatus::closeWait));
    deviceMock.expectOneCall("sendData").ignoreOtherParameters();
    deviceMock.expectOneCall("executeSocketCommand").ignoreOtherParameters();
    deviceMock.expectOneCall("readSocketInterruptRegister").ignoreOtherParameters().andReturnValue(statusSendOk);
    deviceMock.expectOneCall("writeSocketInterruptRegister").ignoreOtherParameters();

    auto buffer = createBuffer(defaultSize);
    const auto result = socket->send(buffer);
    CHECK_EQUAL(buffer.size(), result);
}

TEST(SocketTest, sendChecksFreesizeAndStatusFlagNotEnoughFreeMemory)
{
    constexpr uint16_t freeSize = defaultSize;
    deviceMock.expectOneCall("getTransmitFreeSize")
        .withParameter("socket", socketHandle)
        .andReturnValue(freeSize - 2);
    deviceMock.expectOneCall("readSocketStatusRegister")
        .withParameter("socket", socketHandle)
        .andReturnValue(static_cast<int>(SocketStatus::established));
    deviceMock.expectOneCall("getTransmitFreeSize")
        .withParameter("socket", socketHandle)
        .andReturnValue(freeSize);
    deviceMock.expectOneCall("readSocketStatusRegister")
        .withParameter("socket", socketHandle)
        .andReturnValue(static_cast<int>(SocketStatus::established));
    deviceMock.expectOneCall("sendData").ignoreOtherParameters();
    deviceMock.expectOneCall("executeSocketCommand").ignoreOtherParameters();
    deviceMock.expectOneCall("readSocketInterruptRegister").ignoreOtherParameters().andReturnValue(statusSendOk);
    deviceMock.expectOneCall("writeSocketInterruptRegister").ignoreOtherParameters();

    auto buffer = createBuffer(defaultSize);
    const auto result = socket->send(buffer);
    CHECK_EQUAL(buffer.size(), result);
}

TEST(SocketTest, sendReturnsErrorIfStatusNotEstablished)
{
    constexpr uint16_t freeSize = defaultSize + 2;
    deviceMock.expectOneCall("getTransmitFreeSize")
        .withParameter("socket", socketHandle)
        .andReturnValue(freeSize);
    deviceMock.expectOneCall("readSocketStatusRegister")
        .withParameter("socket", socketHandle)
        .andReturnValue(static_cast<int>(SocketStatus::init));

    auto buffer = createBuffer(defaultSize);
    const auto result = socket->send(buffer);
    CHECK_EQUAL(0, result);
}

TEST(SocketTest, sendSendsDataAndCommand)
{
    auto buffer = createBuffer(defaultSize);
    deviceMock.expectOneCall("getTransmitFreeSize").ignoreOtherParameters().andReturnValue(static_cast<unsigned int>(buffer.size()));
    deviceMock.expectOneCall("readSocketStatusRegister").ignoreOtherParameters().andReturnValue(static_cast<int>(SocketStatus::established));
    deviceMock.expectOneCall("sendData")
        .withParameter("socket", socketHandle)
        .withMemoryBufferParameter("buffer", buffer.data(), buffer.size())
        .withParameter("size", buffer.size());
    deviceMock.expectOneCall("executeSocketCommand")
        .withParameter("socket", socketHandle)
        .withParameter("value", static_cast<int>(SocketCommand::send));
    deviceMock.expectOneCall("readSocketInterruptRegister").ignoreOtherParameters().andReturnValue(statusSendOk);
    deviceMock.expectOneCall("writeSocketInterruptRegister").ignoreOtherParameters();

    const auto result = socket->send(buffer);
    CHECK_EQUAL(buffer.size(), result);
}

TEST(SocketTest, sendWaitsForStatusFlagAfterSend)
{
    auto buffer = createBuffer(defaultSize);
    deviceMock.expectOneCall("getTransmitFreeSize").ignoreOtherParameters().andReturnValue(static_cast<unsigned int>(buffer.size()));
    deviceMock.expectOneCall("readSocketStatusRegister").withParameter("socket", socketHandle).andReturnValue(static_cast<int>(SocketStatus::established));
    deviceMock.expectOneCall("sendData").ignoreOtherParameters();
    deviceMock.expectOneCall("executeSocketCommand").ignoreOtherParameters();
    constexpr uint8_t ready = statusSendOk;
    constexpr uint8_t notReady = !ready;
    deviceMock.expectOneCall("readSocketInterruptRegister")
        .withParameter("socket", socketHandle)
        .andReturnValue(notReady);
    deviceMock.expectOneCall("readSocketStatusRegister")
        .withParameter("socket", socketHandle)
        .andReturnValue(static_cast<int>(SocketStatus::established));
    deviceMock.expectOneCall("readSocketInterruptRegister")
        .withParameter("socket", socketHandle)
        .andReturnValue(notReady);
    deviceMock.expectOneCall("readSocketStatusRegister")
        .withParameter("socket", socketHandle)
        .andReturnValue(static_cast<int>(SocketStatus::established));
    deviceMock.expectOneCall("readSocketInterruptRegister")
        .withParameter("socket", socketHandle)
        .andReturnValue(ready);
    deviceMock.expectOneCall("writeSocketInterruptRegister").ignoreOtherParameters();

    const auto result = socket->send(buffer);
    CHECK_EQUAL(buffer.size(), result);
}

TEST(SocketTest, sendClosesConnectionIfClosedStatus)
{
    auto buffer = createBuffer(defaultSize);
    deviceMock.expectOneCall("getTransmitFreeSize").ignoreOtherParameters().andReturnValue(static_cast<unsigned int>(buffer.size()));
    deviceMock.expectOneCall("readSocketStatusRegister").withParameter("socket", socketHandle).andReturnValue(static_cast<int>(SocketStatus::established));
    deviceMock.expectOneCall("sendData").ignoreOtherParameters();
    deviceMock.expectOneCall("executeSocketCommand").ignoreOtherParameters();
    constexpr uint8_t ready = statusSendOk;
    constexpr uint8_t notReady = !ready;
    deviceMock.expectOneCall("readSocketInterruptRegister")
        .withParameter("socket", socketHandle)
        .andReturnValue(notReady);
    deviceMock.expectOneCall("readSocketStatusRegister")
        .withParameter("socket", socketHandle)
        .andReturnValue(static_cast<int>(SocketStatus::closed));
    expectClose(socketHandle);

    const auto result = socket->send(buffer);
    CHECK_EQUAL(0, result);
}

TEST(SocketTest, sendSetsOkAfterSend)
{
    auto buffer = createBuffer(defaultSize);
    deviceMock.expectOneCall("getTransmitFreeSize")
        .ignoreOtherParameters()
        .andReturnValue(static_cast<unsigned int>(buffer.size()));
    deviceMock.expectOneCall("readSocketStatusRegister")
        .withParameter("socket", socketHandle)
        .andReturnValue(static_cast<int>(SocketStatus::established));
    deviceMock.expectOneCall("sendData").ignoreOtherParameters();
    deviceMock.expectOneCall("executeSocketCommand").ignoreOtherParameters();
    constexpr uint8_t ready = statusSendOk;
    deviceMock.expectOneCall("readSocketInterruptRegister")
        .withParameter("socket", socketHandle)
        .andReturnValue(ready);
    deviceMock.expectOneCall("writeSocketInterruptRegister")
        .withParameter("socket", socketHandle)
        .withParameter("value", statusSendOk);

    const auto result = socket->send(buffer);
    CHECK_EQUAL(buffer.size(), result);
}

TEST(SocketTest, receiveReturnsBytesReceived)
{
    auto buffer = createBuffer(defaultSize);
    deviceMock.expectOneCall("getReceiveFreeSize").ignoreOtherParameters().andReturnValue(100);
    deviceMock.expectOneCall("readSocketStatusRegister")
        .withParameter("socket", socketHandle)
        .andReturnValue(static_cast<int>(SocketStatus::established));
    mock("W5100Device").expectOneCall("receiveData")
        .ignoreOtherParameters()
        .andReturnValue(static_cast<uint16_t>(buffer.size()));
    deviceMock.expectOneCall("executeSocketCommand").ignoreOtherParameters();

    std::array<uint8_t, defaultSize> data;
    const auto result = socket->receive(data);
    CHECK_EQUAL(buffer.size(), result);
}

TEST(SocketTest, receiveLimitsBuffer)
{
    constexpr uint16_t maxReceiveSize = 2048;
    std::array<uint8_t, maxReceiveSize + 1> data;
    deviceMock.expectOneCall("getReceiveFreeSize")
        .ignoreOtherParameters()
        .andReturnValue(maxReceiveSize);
    deviceMock.expectOneCall("readSocketStatusRegister")
        .withParameter("socket", socketHandle)
        .andReturnValue(static_cast<int>(SocketStatus::established));
    mock("W5100Device").expectOneCall("receiveData")
        .ignoreOtherParameters()
        .andReturnValue(maxReceiveSize);
    deviceMock.expectOneCall("executeSocketCommand").ignoreOtherParameters();

    const auto result = socket->receive(data);
    CHECK_EQUAL(maxReceiveSize, result);
}

TEST(SocketTest, receiveLimitsBufferToReceiveFreeSize)
{
    constexpr uint16_t size = defaultSize - 2;
    std::array<uint8_t, defaultSize> data;
    deviceMock.expectOneCall("getReceiveFreeSize")
        .ignoreOtherParameters()
        .andReturnValue(size);
    deviceMock.expectOneCall("readSocketStatusRegister")
        .withParameter("socket", socketHandle)
        .andReturnValue(static_cast<int>(SocketStatus::established));
    mock("W5100Device").expectOneCall("receiveData")
        .ignoreOtherParameters()
        .andReturnValue(size);
    deviceMock.expectOneCall("executeSocketCommand").ignoreOtherParameters();

    const auto result = socket->receive(data);
    CHECK_EQUAL(size, result);
}

TEST(SocketTest, receiveChecksStatusFlagIfEstablished)
{
    auto buffer = createBuffer(defaultSize);
    deviceMock.expectOneCall("getReceiveFreeSize").ignoreOtherParameters().andReturnValue(100);
    deviceMock.expectOneCall("readSocketStatusRegister")
        .withParameter("socket", socketHandle)
        .andReturnValue(static_cast<int>(SocketStatus::established));
    mock("W5100Device").expectOneCall("receiveData")
        .ignoreOtherParameters()
        .andReturnValue(defaultSize);
    deviceMock.expectOneCall("executeSocketCommand").ignoreOtherParameters();

    std::array<uint8_t, defaultSize> data;
    const auto result = socket->receive(data);
    CHECK_EQUAL(buffer.size(), result);
}

TEST(SocketTest, receiveChecksStatusFlagIfCloseWait)
{
    auto buffer = createBuffer(defaultSize);
    deviceMock.expectOneCall("getReceiveFreeSize").ignoreOtherParameters().andReturnValue(100);
    deviceMock.expectOneCall("readSocketStatusRegister")
        .withParameter("socket", socketHandle)
        .andReturnValue(static_cast<int>(SocketStatus::closeWait));
    mock("W5100Device").expectOneCall("receiveData")
        .ignoreOtherParameters()
        .andReturnValue(defaultSize);
    deviceMock.expectOneCall("executeSocketCommand")
        .ignoreOtherParameters();

    std::array<uint8_t, defaultSize> data;
    const auto result = socket->receive(data);
    CHECK_EQUAL(buffer.size(), result);
}

TEST(SocketTest, receiveReturnsErrorIfStatusNotEstablished)
{
    auto buffer = createBuffer(defaultSize);
    deviceMock.expectOneCall("getReceiveFreeSize").ignoreOtherParameters().andReturnValue(100);
    deviceMock.expectOneCall("readSocketStatusRegister")
        .withParameter("socket", socketHandle)
        .andReturnValue(static_cast<int>(SocketStatus::init));

    std::array<uint8_t, defaultSize> data;
    const auto result = socket->receive(data);
    CHECK_EQUAL(0, result);
}

// TODO: Split this test
TEST(SocketTest, receiveReceivesDataAndSendsCommand)
{
    auto buffer = createBuffer(defaultSize);
    gsl::span<uint8_t> bufferSpan(buffer);
    deviceMock.expectOneCall("getReceiveFreeSize").ignoreOtherParameters().andReturnValue(100);
    deviceMock.expectOneCall("readSocketStatusRegister")
        .withParameter("socket", socketHandle)
        .andReturnValue(static_cast<int>(SocketStatus::established));
    deviceMock.expectOneCall("receiveData")
        .withParameter("socket", socketHandle)
        .withOutputParameterReturning("buffer", buffer.data(), buffer.size())
        .withParameter("size", buffer.size())
        .ignoreOtherParameters()
        .andReturnValue(defaultSize);
    deviceMock.expectOneCall("executeSocketCommand")
        .withParameter("socket", socketHandle)
        .withParameter("value", static_cast<int>(SocketCommand::receive));

    std::array<uint8_t, defaultSize> data;
    const auto result = socket->receive(data);
    CHECK_EQUAL(buffer.size(), result);
}

TEST(SocketTest, receiveWaitsForStatusFlagAfterSend)
{
    auto buffer = createBuffer(defaultSize);
    deviceMock.expectOneCall("getReceiveFreeSize")
        .ignoreOtherParameters()
        .andReturnValue(100);
    deviceMock.expectOneCall("readSocketStatusRegister")
        .withParameter("socket", socketHandle)
        .andReturnValue(static_cast<int>(SocketStatus::established));
    deviceMock.expectOneCall("receiveData")
        .ignoreOtherParameters()
        .andReturnValue(defaultSize);
    deviceMock.expectOneCall("executeSocketCommand")
        .ignoreOtherParameters();

    std::array<uint8_t, defaultSize> data;
    const auto result = socket->receive(data);
    CHECK_EQUAL(buffer.size(), result);
}

TEST(SocketTest, receiveWaitsForDataAvailable)
{
    auto buffer = createBuffer(defaultSize);
    deviceMock.expectOneCall("getReceiveFreeSize")
        .ignoreOtherParameters()
        .andReturnValue(0);
    deviceMock.expectOneCall("readSocketStatusRegister")
        .withParameter("socket", socketHandle)
        .andReturnValue(static_cast<int>(SocketStatus::established));

    deviceMock.expectOneCall("getReceiveFreeSize")
        .ignoreOtherParameters()
        .andReturnValue(defaultSize);
    deviceMock.expectOneCall("readSocketStatusRegister")
        .withParameter("socket", socketHandle)
        .andReturnValue(static_cast<int>(SocketStatus::established));
    deviceMock.expectOneCall("receiveData")
        .withParameter("socket", socketHandle)
        .withParameter("size", defaultSize)
        .ignoreOtherParameters()
        .andReturnValue(defaultSize);
    deviceMock.expectOneCall("executeSocketCommand")
        .withParameter("socket", socketHandle)
        .withParameter("value", static_cast<int>(SocketCommand::receive));


    std::array<uint8_t, defaultSize> data;
    const auto result = socket->receive(data);
    CHECK_EQUAL(buffer.size(), result);

}

TEST(SocketTest, getStatus)
{
    deviceMock.expectOneCall("readSocketStatusRegister")
        .withParameter("socket", socketHandle)
        .andReturnValue(static_cast<int>(SocketStatus::listen));
    const auto status = socket->getStatus();
    CHECK_EQUAL(eth::SocketStatus::listen, status);
}
