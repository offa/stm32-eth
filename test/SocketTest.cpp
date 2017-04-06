/*
 * Stm32 Eth - Ethernet connectivity for Stm32
 * Copyright (C) 2016 - 2017  offa
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
#include "w5100/Device.h"
#include "spi/SpiWriter.h"
#include "TestHelper.h"
#include <vector>
#include <numeric>
#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>

using eth::SocketStatus;
using eth::SocketCommand;
using eth::SocketInterrupt;
using eth::Socket;
using eth::SocketHandle;
using eth::Protocol;
using eth::w5100::Device;
using eth::spi::SpiWriter;


namespace
{
    constexpr SocketHandle socketHandle = eth::makeHandle<0>();
}


TEST_GROUP(SocketTest)
{

    void setup() override
    {
        socket = std::make_unique<Socket>(socketHandle, device);
        mock().strictOrder();
    }

    void teardown() override
    {
        ignoreDestruction();
        mock().checkExpectations();
        mock().clear();
    }

    void expectClose(SocketHandle handle) const
    {
        expectSocketCommand(handle, SocketCommand::close);
        deviceMock.expectOneCall("writeSocketInterruptRegister")
            .withParameter("socket", handle.value())
            .withParameter("value", 0xff);
        expectSocketStatusRead(handle, SocketStatus::closed);
    }

    void expectSocketInterruptRead(SocketHandle s, std::uint8_t value) const
    {
        deviceMock.expectOneCall("readSocketInterruptRegister")
            .withParameter("socket", s.value())
            .andReturnValue(value);
    }

    void expectSocketInterruptRead(SocketHandle s, SocketInterrupt::Mask value) const
    {
        expectSocketInterruptRead(s, static_cast<std::uint8_t>(value));
    }

    void expectSocketStatusRead(SocketHandle s, SocketStatus status) const
    {
        deviceMock.expectOneCall("readSocketStatusRegister")
            .withParameter("socket", s.value())
            .andReturnValue(static_cast<std::uint8_t>(status));
    }

    void expectSocketCommand(SocketHandle s, SocketCommand cmd) const
    {
        deviceMock.expectOneCall("executeSocketCommand")
            .withParameter("socket", s.value())
            .withParameter("value", static_cast<std::uint8_t>(cmd));
    }

    std::vector<std::uint8_t> createBuffer(std::size_t size) const
    {
        std::vector<std::uint8_t> buffer(size);
        std::iota(buffer.begin(), buffer.end(), 0);
        return buffer;
    }

    void ignoreDestruction()
    {
        auto f = gsl::finally([] { mock().enable(); });
        mock().disable();
        socket.reset();
    }


    std::unique_ptr<Socket> socket;
    SpiWriter spi{eth::spi::spi2};
    Device device{spi};
    MockSupport& deviceMock = mock("Device");
    MockSupport& platformMock = mock("platform");
    static constexpr std::uint16_t port = 1234;
    static constexpr Protocol protocol = Protocol::tcp;
    static constexpr std::uint8_t flag = 0;
    static constexpr auto statusSendOk = static_cast<std::uint8_t>(SocketInterrupt::Mask::send);
    static constexpr std::uint16_t defaultSize = 10;
};

TEST(SocketTest, openReturnsErrorOnUnsupportedProtocol)
{
    constexpr Protocol invalidProtocol = Protocol::pppoe;
    const auto result = socket->open(invalidProtocol, port, flag);
    CHECK_EQUAL(Socket::Status::failed, result);
}

TEST(SocketTest, openReturnsSuccess)
{
    expectClose(socketHandle);
    deviceMock.expectOneCall("writeSocketModeRegister").ignoreOtherParameters();
    deviceMock.expectOneCall("writeSocketSourcePort").ignoreOtherParameters();
    deviceMock.expectOneCall("executeSocketCommand").ignoreOtherParameters();
    expectSocketStatusRead(socketHandle, SocketStatus::established);

    const auto result = socket->open(protocol, port, flag);
    CHECK_EQUAL(Socket::Status::ok, result);
}

TEST(SocketTest, openResetsSocketFirst)
{
    expectClose(socketHandle);
    deviceMock.expectOneCall("writeSocketModeRegister").ignoreOtherParameters();
    deviceMock.expectOneCall("writeSocketSourcePort").ignoreOtherParameters();
    deviceMock.expectOneCall("executeSocketCommand").ignoreOtherParameters();
    expectSocketStatusRead(socketHandle, SocketStatus::established);

    const auto result = socket->open(protocol, port, flag);
    CHECK_EQUAL(Socket::Status::ok, result);
}

TEST(SocketTest, openSetsProtocol)
{
    constexpr std::uint8_t value = static_cast<std::uint8_t>(protocol) | flag;
    expectClose(socketHandle);
    deviceMock.expectOneCall("writeSocketModeRegister")
        .withParameter("socket", socketHandle.value())
        .withParameter("value", value);
    deviceMock.expectOneCall("writeSocketSourcePort").ignoreOtherParameters();
    deviceMock.expectOneCall("executeSocketCommand").ignoreOtherParameters();
    expectSocketStatusRead(socketHandle, SocketStatus::established);

    const auto result = socket->open(protocol, port, flag);
    CHECK_EQUAL(Socket::Status::ok, result);
}

TEST(SocketTest, openSetsFlag)
{
    constexpr std::uint8_t flagValue = 0x0a;
    constexpr std::uint8_t value = static_cast<std::uint8_t>(protocol) | flagValue;
    expectClose(socketHandle);
    deviceMock.expectOneCall("writeSocketModeRegister")
        .withParameter("socket", socketHandle.value())
        .withParameter("value", value);
    deviceMock.expectOneCall("writeSocketSourcePort").ignoreOtherParameters();
    deviceMock.expectOneCall("executeSocketCommand").ignoreOtherParameters();
    expectSocketStatusRead(socketHandle, SocketStatus::established);

    const auto result = socket->open(protocol, port, flagValue);
    CHECK_EQUAL(Socket::Status::ok, result);
}

TEST(SocketTest, openSetsPort)
{
    expectClose(socketHandle);
    deviceMock.expectOneCall("writeSocketModeRegister").ignoreOtherParameters();
    deviceMock.expectOneCall("writeSocketSourcePort")
        .withParameter("socket", socketHandle.value())
        .withParameter("value", port);
    deviceMock.expectOneCall("executeSocketCommand").ignoreOtherParameters();
    expectSocketStatusRead(socketHandle, SocketStatus::established);

    const auto result = socket->open(protocol, port, flag);
    CHECK_EQUAL(Socket::Status::ok, result);
}

TEST(SocketTest, openOpensSocket)
{
    expectClose(socketHandle);
    deviceMock.expectOneCall("writeSocketModeRegister").ignoreOtherParameters();
    deviceMock.expectOneCall("writeSocketSourcePort").ignoreOtherParameters();
    expectSocketCommand(socketHandle, SocketCommand::open);
    expectSocketStatusRead(socketHandle, SocketStatus::established);

    const auto result = socket->open(protocol, port, flag);
    CHECK_EQUAL(Socket::Status::ok, result);
}

TEST(SocketTest, openWaitsForStatus)
{
    expectClose(socketHandle);
    deviceMock.expectOneCall("writeSocketModeRegister").ignoreOtherParameters();
    deviceMock.expectOneCall("writeSocketSourcePort").ignoreOtherParameters();
    deviceMock.expectOneCall("executeSocketCommand").ignoreOtherParameters();
    expectSocketStatusRead(socketHandle, SocketStatus::closed);
    expectSocketStatusRead(socketHandle, SocketStatus::closed);
    expectSocketStatusRead(socketHandle, SocketStatus::established);

    const auto result = socket->open(protocol, port, flag);
    CHECK_EQUAL(Socket::Status::ok, result);
}

TEST(SocketTest, close)
{
    expectClose(socketHandle);
    socket->close();
}

TEST(SocketTest, closeWaitsForCloseStatus)
{
    deviceMock.expectOneCall("executeSocketCommand").ignoreOtherParameters();
    deviceMock.expectOneCall("writeSocketInterruptRegister").ignoreOtherParameters();
    expectSocketStatusRead(socketHandle, SocketStatus::established);
    expectSocketStatusRead(socketHandle, SocketStatus::established);
    expectSocketStatusRead(socketHandle, SocketStatus::closed);

    socket->close();
}

TEST(SocketTest, closedOnDestruction)
{
    expectSocketCommand(socketHandle, SocketCommand::close);
    deviceMock.expectOneCall("writeSocketInterruptRegister")
        .withParameter("socket", socketHandle.value())
        .withParameter("value", 0xff);
    Socket s(socketHandle, device);
}

TEST(SocketTest, listenReturnsErrorIfStatusNotInit)
{
    expectSocketStatusRead(socketHandle, SocketStatus::closed);

    const auto result = socket->listen();
    CHECK_EQUAL(Socket::Status::closed, result);
}

TEST(SocketTest, listenListens)
{
    expectSocketStatusRead(socketHandle, SocketStatus::init);
    expectSocketCommand(socketHandle, SocketCommand::listen);

    const auto result = socket->listen();
    CHECK_EQUAL(Socket::Status::ok, result);
}

TEST(SocketTest, acceptWaitsBetweenStatusCheck)
{
    constexpr std::uint32_t waitTime = 100;
    expectSocketStatusRead(socketHandle, SocketStatus::listen);
    expectSocketStatusRead(socketHandle, SocketStatus::established);
    platformMock.expectOneCall("wait").withParameter("timeMs", waitTime);
    socket->accept();
}

TEST(SocketTest, acceptWaitsForConnection)
{
    platformMock.expectNCalls(3, "wait").ignoreOtherParameters();
    expectSocketStatusRead(socketHandle, SocketStatus::listen);
    expectSocketStatusRead(socketHandle, SocketStatus::listen);
    expectSocketStatusRead(socketHandle, SocketStatus::listen);
    expectSocketStatusRead(socketHandle, SocketStatus::established);

    socket->accept();
}

TEST(SocketTest, sendReturnsBytesTransmitted)
{
    deviceMock.expectOneCall("getTransmitFreeSize").ignoreOtherParameters().andReturnValue(100);
    expectSocketStatusRead(socketHandle, SocketStatus::established);
    deviceMock.expectOneCall("sendData").ignoreOtherParameters();
    deviceMock.expectOneCall("executeSocketCommand").ignoreOtherParameters();

    auto buffer = createBuffer(defaultSize);
    const auto result = socket->send(buffer);
    CHECK_EQUAL(buffer.size(), result);
}

TEST(SocketTest, sendIgnoresEmptyBuffer)
{
    const auto result = socket->send({ });
    CHECK_EQUAL(0, result);
}

TEST(SocketTest, sendIgnoresNullptrBuffer)
{
    const auto result = socket->send(nullptr);
    CHECK_EQUAL(0, result);
}

TEST(SocketTest, sendLimitsBufferSize)
{
    constexpr std::uint16_t maxSendSize = 2048;
    deviceMock.expectOneCall("getTransmitFreeSize").ignoreOtherParameters().andReturnValue(maxSendSize);
    expectSocketStatusRead(socketHandle, SocketStatus::established);
    deviceMock.expectOneCall("sendData").ignoreOtherParameters();
    deviceMock.expectOneCall("executeSocketCommand").ignoreOtherParameters();

    auto buffer = createBuffer(maxSendSize + 1);
    const auto result = socket->send(buffer);
    CHECK_EQUAL(maxSendSize, result);
}

TEST(SocketTest, sendChecksFreesizeAndStatusFlagIfEstablished)
{
    constexpr std::uint16_t freeSize = defaultSize + 2;
    deviceMock.expectOneCall("getTransmitFreeSize")
        .withParameter("socket", socketHandle.value())
        .andReturnValue(freeSize);
    expectSocketStatusRead(socketHandle, SocketStatus::established);
    deviceMock.expectOneCall("sendData").ignoreOtherParameters();
    deviceMock.expectOneCall("executeSocketCommand").ignoreOtherParameters();

    auto buffer = createBuffer(defaultSize);
    const auto result = socket->send(buffer);
    CHECK_EQUAL(buffer.size(), result);
}

TEST(SocketTest, sendChecksFreesizeAndStatusFlagIfCloseWait)
{
    constexpr std::uint16_t freeSize = defaultSize + 2;
    deviceMock.expectOneCall("getTransmitFreeSize")
        .withParameter("socket", socketHandle.value())
        .andReturnValue(freeSize);
    expectSocketStatusRead(socketHandle, SocketStatus::closeWait);
    deviceMock.expectOneCall("sendData").ignoreOtherParameters();
    deviceMock.expectOneCall("executeSocketCommand").ignoreOtherParameters();

    auto buffer = createBuffer(defaultSize);
    const auto result = socket->send(buffer);
    CHECK_EQUAL(buffer.size(), result);
}

TEST(SocketTest, sendChecksFreesizeAndStatusFlagNotEnoughFreeMemory)
{
    constexpr std::uint16_t freeSize = defaultSize;
    deviceMock.expectOneCall("getTransmitFreeSize")
        .withParameter("socket", socketHandle.value())
        .andReturnValue(freeSize - 2);
    expectSocketStatusRead(socketHandle, SocketStatus::established);
    deviceMock.expectOneCall("getTransmitFreeSize")
        .withParameter("socket", socketHandle.value())
        .andReturnValue(freeSize);
    expectSocketStatusRead(socketHandle, SocketStatus::established);
    deviceMock.expectOneCall("sendData").ignoreOtherParameters();
    deviceMock.expectOneCall("executeSocketCommand").ignoreOtherParameters();

    auto buffer = createBuffer(defaultSize);
    const auto result = socket->send(buffer);
    CHECK_EQUAL(buffer.size(), result);
}

TEST(SocketTest, sendReturnsErrorIfStatusNotEstablished)
{
    expectSocketStatusRead(socketHandle, SocketStatus::init);

    auto buffer = createBuffer(defaultSize);
    const auto result = socket->send(buffer);
    CHECK_EQUAL(0, result);
}

TEST(SocketTest, sendSendsDataAndCommand)
{
    auto buffer = createBuffer(defaultSize);
    deviceMock.expectOneCall("getTransmitFreeSize").ignoreOtherParameters().andReturnValue(static_cast<unsigned int>(buffer.size()));
    expectSocketStatusRead(socketHandle, SocketStatus::established);
    deviceMock.expectOneCall("sendData")
        .withParameter("socket", socketHandle.value())
        .withMemoryBufferParameter("buffer", buffer.data(), buffer.size())
        .withParameter("size", buffer.size());
    expectSocketCommand(socketHandle, SocketCommand::send);

    const auto result = socket->send(buffer);
    CHECK_EQUAL(buffer.size(), result);
}

TEST(SocketTest, sendWaitsForStatusFlagAfterSend)
{
    auto buffer = createBuffer(defaultSize);
    deviceMock.expectOneCall("getTransmitFreeSize").ignoreOtherParameters().andReturnValue(static_cast<unsigned int>(buffer.size()));
    expectSocketStatusRead(socketHandle, SocketStatus::established);
    deviceMock.expectOneCall("sendData").ignoreOtherParameters();
    deviceMock.expectOneCall("executeSocketCommand").ignoreOtherParameters();

    const auto result = socket->send(buffer);
    CHECK_EQUAL(buffer.size(), result);
}

TEST(SocketTest, sendSetsOkAfterSend)
{
    auto buffer = createBuffer(defaultSize);
    deviceMock.expectOneCall("getTransmitFreeSize")
        .ignoreOtherParameters()
        .andReturnValue(static_cast<unsigned int>(buffer.size()));
    expectSocketStatusRead(socketHandle, SocketStatus::established);
    deviceMock.expectOneCall("sendData").ignoreOtherParameters();
    deviceMock.expectOneCall("executeSocketCommand").ignoreOtherParameters();

    const auto result = socket->send(buffer);
    CHECK_EQUAL(buffer.size(), result);
}

TEST(SocketTest, receiveReturnsBytesReceived)
{
    auto buffer = createBuffer(defaultSize);
    deviceMock.expectOneCall("getReceiveFreeSize").ignoreOtherParameters().andReturnValue(100);
    expectSocketStatusRead(socketHandle, SocketStatus::established);
    mock("Device").expectOneCall("receiveData")
        .ignoreOtherParameters()
        .andReturnValue(static_cast<std::uint16_t>(buffer.size()));
    deviceMock.expectOneCall("executeSocketCommand").ignoreOtherParameters();

    std::array<std::uint8_t, defaultSize> data;
    const auto result = socket->receive(data);
    CHECK_EQUAL(buffer.size(), result);
}

TEST(SocketTest, receiveIgnoresEmptyBuffer)
{
    std::vector<std::uint8_t> buffer;
    const auto result = socket->receive(buffer);
    CHECK_EQUAL(0, result);
}

TEST(SocketTest, receiveIgnoresNullptrBuffer)
{
    const auto result = socket->receive(nullptr);
    CHECK_EQUAL(0, result);
}

TEST(SocketTest, receiveLimitsBuffer)
{
    constexpr std::uint16_t maxReceiveSize = 2048;
    std::array<std::uint8_t, maxReceiveSize + 1> data;
    deviceMock.expectOneCall("getReceiveFreeSize")
        .ignoreOtherParameters()
        .andReturnValue(maxReceiveSize);
    expectSocketStatusRead(socketHandle, SocketStatus::established);
    mock("Device").expectOneCall("receiveData")
        .ignoreOtherParameters()
        .andReturnValue(maxReceiveSize);
    deviceMock.expectOneCall("executeSocketCommand").ignoreOtherParameters();

    const auto result = socket->receive(data);
    CHECK_EQUAL(maxReceiveSize, result);
}

TEST(SocketTest, receiveLimitsBufferToReceiveFreeSize)
{
    constexpr std::uint16_t size = defaultSize - 2;
    std::array<std::uint8_t, defaultSize> data;
    deviceMock.expectOneCall("getReceiveFreeSize")
        .ignoreOtherParameters()
        .andReturnValue(size);
    expectSocketStatusRead(socketHandle, SocketStatus::established);
    mock("Device").expectOneCall("receiveData")
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
    expectSocketStatusRead(socketHandle, SocketStatus::established);
    mock("Device").expectOneCall("receiveData")
        .ignoreOtherParameters()
        .andReturnValue(defaultSize);
    deviceMock.expectOneCall("executeSocketCommand").ignoreOtherParameters();

    std::array<std::uint8_t, defaultSize> data;
    const auto result = socket->receive(data);
    CHECK_EQUAL(buffer.size(), result);
}

TEST(SocketTest, receiveChecksStatusFlagIfCloseWait)
{
    auto buffer = createBuffer(defaultSize);
    deviceMock.expectOneCall("getReceiveFreeSize").ignoreOtherParameters().andReturnValue(100);
    expectSocketStatusRead(socketHandle, SocketStatus::closeWait);
    mock("Device").expectOneCall("receiveData")
        .ignoreOtherParameters()
        .andReturnValue(defaultSize);
    deviceMock.expectOneCall("executeSocketCommand").ignoreOtherParameters();

    std::array<std::uint8_t, defaultSize> data;
    const auto result = socket->receive(data);
    CHECK_EQUAL(buffer.size(), result);
}

TEST(SocketTest, receiveReturnsErrorIfStatusNotEstablished)
{
    auto buffer = createBuffer(defaultSize);
    expectSocketStatusRead(socketHandle, SocketStatus::init);

    std::array<std::uint8_t, defaultSize> data;
    const auto result = socket->receive(data);
    CHECK_EQUAL(0, result);
}

TEST(SocketTest, receiveReceivesData)
{
    auto buffer = createBuffer(defaultSize);
    gsl::span<std::uint8_t> bufferSpan(buffer);
    deviceMock.expectOneCall("getReceiveFreeSize").ignoreOtherParameters().andReturnValue(100);
    expectSocketStatusRead(socketHandle, SocketStatus::established);
    deviceMock.expectOneCall("receiveData")
        .withParameter("socket", socketHandle.value())
        .withOutputParameterReturning("buffer", buffer.data(), buffer.size())
        .withParameter("size", buffer.size())
        .andReturnValue(defaultSize);
    deviceMock.expectOneCall("executeSocketCommand").ignoreOtherParameters();

    std::array<std::uint8_t, defaultSize> data;
    const auto result = socket->receive(data);
    CHECK_EQUAL(buffer.size(), result);
}

TEST(SocketTest, receiveSendsCommandAfterReceive)
{
    auto buffer = createBuffer(defaultSize);
    gsl::span<std::uint8_t> bufferSpan(buffer);
    deviceMock.expectOneCall("getReceiveFreeSize").ignoreOtherParameters().andReturnValue(100);
    expectSocketStatusRead(socketHandle, SocketStatus::established);
    deviceMock.expectOneCall("receiveData")
        .ignoreOtherParameters()
        .andReturnValue(defaultSize);
    expectSocketCommand(socketHandle, SocketCommand::receive);

    std::array<std::uint8_t, defaultSize> data;
    const auto result = socket->receive(data);
    CHECK_EQUAL(buffer.size(), result);
}

TEST(SocketTest, receiveWaitsForStatusFlagAfterSend)
{
    auto buffer = createBuffer(defaultSize);
    deviceMock.expectOneCall("getReceiveFreeSize")
        .ignoreOtherParameters()
        .andReturnValue(100);
    expectSocketStatusRead(socketHandle, SocketStatus::established);
    deviceMock.expectOneCall("receiveData")
        .ignoreOtherParameters()
        .andReturnValue(defaultSize);
    deviceMock.expectOneCall("executeSocketCommand")
        .ignoreOtherParameters();

    std::array<std::uint8_t, defaultSize> data;
    const auto result = socket->receive(data);
    CHECK_EQUAL(buffer.size(), result);
}

TEST(SocketTest, receiveWaitsForDataAvailable)
{
    auto buffer = createBuffer(defaultSize);
    deviceMock.expectOneCall("getReceiveFreeSize")
        .ignoreOtherParameters()
        .andReturnValue(0);
    expectSocketStatusRead(socketHandle, SocketStatus::established);

    deviceMock.expectOneCall("getReceiveFreeSize")
        .ignoreOtherParameters()
        .andReturnValue(defaultSize);
    expectSocketStatusRead(socketHandle, SocketStatus::established);
    deviceMock.expectOneCall("receiveData")
        .withParameter("socket", socketHandle.value())
        .withParameter("size", defaultSize)
        .ignoreOtherParameters()
        .andReturnValue(defaultSize);
    expectSocketCommand(socketHandle, SocketCommand::receive);

    std::array<std::uint8_t, defaultSize> data;
    const auto result = socket->receive(data);
    CHECK_EQUAL(buffer.size(), result);

}

TEST(SocketTest, getStatus)
{
    expectSocketStatusRead(socketHandle, SocketStatus::listen);
    const auto status = socket->getStatus();
    CHECK_EQUAL(eth::SocketStatus::listen, status);
}

TEST(SocketTest, connect)
{
    std::array<std::uint8_t, 4> addr = {{127, 0, 0, 1}};
    constexpr std::uint16_t port = 4567;

    deviceMock.expectOneCall("setDestAddress")
                .withParameter("socket", socketHandle.value())
                .withMemoryBufferParameter("buffer", addr.data(), addr.size())
                .withParameter("port", port);
    expectSocketCommand(socketHandle, SocketCommand::connect);
    expectSocketStatusRead(socketHandle, SocketStatus::established);

    const auto rtn = socket->connect(addr, port);
    CHECK_EQUAL(Socket::Status::ok, rtn);
}

TEST(SocketTest, connectWaitsForEstablishedStatus)
{
    std::array<std::uint8_t, 4> addr = {{127, 0, 0, 1}};

    deviceMock.expectOneCall("setDestAddress").ignoreOtherParameters();
    deviceMock.expectOneCall("executeSocketCommand").ignoreOtherParameters();
    expectSocketStatusRead(socketHandle, SocketStatus::init);
    expectSocketStatusRead(socketHandle, SocketStatus::init);
    expectSocketInterruptRead(socketHandle, 0x00u);
    expectSocketStatusRead(socketHandle, SocketStatus::init);
    expectSocketStatusRead(socketHandle, SocketStatus::init);
    expectSocketInterruptRead(socketHandle, 0x00u);
    expectSocketStatusRead(socketHandle, SocketStatus::established);

    const auto rtn = socket->connect(addr, 4567);
    CHECK_EQUAL(Socket::Status::ok, rtn);
}

TEST(SocketTest, connectErrorOnClosedConnection)
{
    std::array<std::uint8_t, 4> addr = {{127, 0, 0, 1}};

    deviceMock.expectOneCall("setDestAddress").ignoreOtherParameters();
    deviceMock.expectOneCall("executeSocketCommand").ignoreOtherParameters();
    expectSocketStatusRead(socketHandle, SocketStatus::init);
    expectSocketStatusRead(socketHandle, SocketStatus::closed);

    const auto rtn = socket->connect(addr, 4567);
    CHECK_EQUAL(Socket::Status::closed, rtn);
}

TEST(SocketTest, connectErrorOnTimeout)
{
    std::array<std::uint8_t, 4> addr = {{127, 0, 0, 1}};

    deviceMock.expectOneCall("setDestAddress").ignoreOtherParameters();
    deviceMock.expectOneCall("executeSocketCommand").ignoreOtherParameters();
    expectSocketStatusRead(socketHandle, SocketStatus::init);
    expectSocketStatusRead(socketHandle, SocketStatus::init);
    expectSocketInterruptRead(socketHandle, SocketInterrupt::Mask::timeout);

    const auto rtn = socket->connect(addr, 4567);
    CHECK_EQUAL(Socket::Status::timeout, rtn);
}

TEST(SocketTest, disconnect)
{
    expectSocketCommand(socketHandle, SocketCommand::disconnect);
    expectSocketStatusRead(socketHandle, SocketStatus::closed);

    const auto rtn = socket->disconnect();
    CHECK_EQUAL(Socket::Status::ok, rtn);
}

TEST(SocketTest, disconnectWaitsForClosedStatus)
{
    expectSocketCommand(socketHandle, SocketCommand::disconnect);
    expectSocketStatusRead(socketHandle, SocketStatus::established);
    expectSocketInterruptRead(socketHandle, 0x00u);
    expectSocketStatusRead(socketHandle, SocketStatus::established);
    expectSocketInterruptRead(socketHandle, 0x00u);
    expectSocketStatusRead(socketHandle, SocketStatus::closed);

    const auto rtn = socket->disconnect();
    CHECK_EQUAL(Socket::Status::ok, rtn);
}

TEST(SocketTest, disconnectErrorOnTimeout)
{
    expectSocketCommand(socketHandle, SocketCommand::disconnect);
    expectSocketStatusRead(socketHandle, SocketStatus::established);
    expectSocketInterruptRead(socketHandle, SocketInterrupt::Mask::timeout);

    const auto rtn = socket->disconnect();
    CHECK_EQUAL(Socket::Status::timeout, rtn);
}

