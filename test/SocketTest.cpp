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

#include "Socket.h"
#include "SocketStatus.h"
#include "SocketCommand.h"
#include "SocketInterrupt.h"
#include "w5100/Device.h"
#include "spi/SpiWriter.h"
#include "TestHelper.h"
#include <vector>
#include <memory>
#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>

using eth::SocketStatus;
using eth::SocketCommand;
using eth::SocketInterrupt;
using eth::Socket;
using eth::SocketHandle;
using eth::NetAddress;
using eth::Protocol;
using eth::w5100::Device;
using eth::spi::SpiWriter;


namespace
{
    constexpr SocketHandle socketHandle = eth::makeHandle<0>();
}


TEST_GROUP(SocketTest)
{
    enum class Mode { receive, send };

    void setup() override
    {
        device = std::make_unique<Device>(spi);
        socket = std::make_unique<Socket>(socketHandle, *device);
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
        mock("Device").expectOneCall("writeSocketInterruptRegister")
            .withParameter("socket", handle.value())
            .withParameter("value", 0xff);
        expectSocketStatusRead(handle, SocketStatus::closed);
    }

    void expectSocketInterruptRead(SocketHandle s, std::uint8_t value) const
    {
        mock("Device").expectOneCall("readSocketInterruptRegister")
            .withParameter("socket", s.value())
            .andReturnValue(value);
    }

    void expectSocketInterruptRead(SocketHandle s, SocketInterrupt::Mask value) const
    {
        expectSocketInterruptRead(s, static_cast<std::uint8_t>(value));
    }

    void expectSocketStatusRead(SocketHandle s, SocketStatus status) const
    {
        mock("Device").expectOneCall("readSocketStatusRegister")
            .withParameter("socket", s.value())
            .andReturnValue(static_cast<std::uint8_t>(status));
    }

    void expectSocketCommand(SocketHandle s, SocketCommand cmd) const
    {
        mock("Device").expectOneCall("executeSocketCommand")
            .withParameter("socket", s.value())
            .withParameter("value", static_cast<std::uint8_t>(cmd));
    }

    void expectWaitForFreeRxTx(Mode mode, SocketHandle handle, std::uint16_t freeSize) const
    {
        expectSocketStatusRead(handle, SocketStatus::established);
        const auto fn = ( mode == Mode::receive
                            ? "getReceiveFreeSize"
                            : "getTransmitFreeSize" );
        mock("Device").expectOneCall(fn)
            .withParameter("socket", handle.value())
            .andReturnValue(freeSize);
    }

    void ignoreDestruction()
    {
        auto f = gsl::finally([] { mock().enable(); });
        mock().disable();
        socket.reset();
    }


    std::unique_ptr<Socket> socket;
    SpiWriter spi{eth::spi::spi2};
    std::unique_ptr<Device> device;
    static constexpr std::uint16_t port{1234};
    static constexpr Protocol protocol = Protocol::tcp;
    static constexpr std::uint8_t flag{0};
    static constexpr auto statusSendOk = static_cast<std::uint8_t>(SocketInterrupt::Mask::send);
    static constexpr std::uint16_t defaultSize{10};
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
    mock("Device").expectOneCall("writeSocketModeRegister").ignoreOtherParameters();
    mock("Device").expectOneCall("writeSocketSourcePort").ignoreOtherParameters();
    mock("Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();
    expectSocketStatusRead(socketHandle, SocketStatus::established);

    const auto result = socket->open(protocol, port, flag);
    CHECK_EQUAL(Socket::Status::ok, result);
}

TEST(SocketTest, openResetsSocketFirst)
{
    expectClose(socketHandle);
    mock("Device").expectOneCall("writeSocketModeRegister").ignoreOtherParameters();
    mock("Device").expectOneCall("writeSocketSourcePort").ignoreOtherParameters();
    mock("Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();
    expectSocketStatusRead(socketHandle, SocketStatus::established);

    const auto result = socket->open(protocol, port, flag);
    CHECK_EQUAL(Socket::Status::ok, result);
}

TEST(SocketTest, openSetsProtocol)
{
    constexpr std::uint8_t value = static_cast<std::uint8_t>(protocol) | flag;
    expectClose(socketHandle);
    mock("Device").expectOneCall("writeSocketModeRegister")
        .withParameter("socket", socketHandle.value())
        .withParameter("value", value);
    mock("Device").expectOneCall("writeSocketSourcePort").ignoreOtherParameters();
    mock("Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();
    expectSocketStatusRead(socketHandle, SocketStatus::established);

    const auto result = socket->open(protocol, port, flag);
    CHECK_EQUAL(Socket::Status::ok, result);
}

TEST(SocketTest, openSetsFlag)
{
    constexpr std::uint8_t flagValue = 0x0a;
    constexpr std::uint8_t value = static_cast<std::uint8_t>(protocol) | flagValue;
    expectClose(socketHandle);
    mock("Device").expectOneCall("writeSocketModeRegister")
        .withParameter("socket", socketHandle.value())
        .withParameter("value", value);
    mock("Device").expectOneCall("writeSocketSourcePort").ignoreOtherParameters();
    mock("Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();
    expectSocketStatusRead(socketHandle, SocketStatus::established);

    const auto result = socket->open(protocol, port, flagValue);
    CHECK_EQUAL(Socket::Status::ok, result);
}

TEST(SocketTest, openSetsPort)
{
    expectClose(socketHandle);
    mock("Device").expectOneCall("writeSocketModeRegister").ignoreOtherParameters();
    mock("Device").expectOneCall("writeSocketSourcePort")
        .withParameter("socket", socketHandle.value())
        .withParameter("value", port);
    mock("Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();
    expectSocketStatusRead(socketHandle, SocketStatus::established);

    const auto result = socket->open(protocol, port, flag);
    CHECK_EQUAL(Socket::Status::ok, result);
}

TEST(SocketTest, openOpensSocket)
{
    expectClose(socketHandle);
    mock("Device").expectOneCall("writeSocketModeRegister").ignoreOtherParameters();
    mock("Device").expectOneCall("writeSocketSourcePort").ignoreOtherParameters();
    expectSocketCommand(socketHandle, SocketCommand::open);
    expectSocketStatusRead(socketHandle, SocketStatus::established);

    const auto result = socket->open(protocol, port, flag);
    CHECK_EQUAL(Socket::Status::ok, result);
}

TEST(SocketTest, openWaitsForStatus)
{
    expectClose(socketHandle);
    mock("Device").expectOneCall("writeSocketModeRegister").ignoreOtherParameters();
    mock("Device").expectOneCall("writeSocketSourcePort").ignoreOtherParameters();
    mock("Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();
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
    mock("Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();
    mock("Device").expectOneCall("writeSocketInterruptRegister").ignoreOtherParameters();
    expectSocketStatusRead(socketHandle, SocketStatus::established);
    expectSocketStatusRead(socketHandle, SocketStatus::established);
    expectSocketStatusRead(socketHandle, SocketStatus::closed);

    socket->close();
}

TEST(SocketTest, closedOnDestruction)
{
    expectSocketCommand(socketHandle, SocketCommand::close);
    mock("Device").expectOneCall("writeSocketInterruptRegister")
        .withParameter("socket", socketHandle.value())
        .withParameter("value", 0xff);
    Socket s(socketHandle, *device);
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
    mock("platform").expectOneCall("wait").withParameter("timeMs", waitTime);
    socket->accept();
}

TEST(SocketTest, acceptWaitsForConnection)
{
    mock("platform").expectNCalls(3, "wait").ignoreOtherParameters();
    expectSocketStatusRead(socketHandle, SocketStatus::listen);
    expectSocketStatusRead(socketHandle, SocketStatus::listen);
    expectSocketStatusRead(socketHandle, SocketStatus::listen);
    expectSocketStatusRead(socketHandle, SocketStatus::established);

    socket->accept();
}

TEST(SocketTest, sendReturnsBytesTransmitted)
{
    expectWaitForFreeRxTx(Mode::send, socketHandle, 100);
    mock("Device").expectOneCall("sendData").ignoreOtherParameters();
    mock("Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();

    auto buffer = createBuffer(defaultSize);
    const auto result = socket->send(buffer);
    CHECK_EQUAL(buffer.size(), result);
}

TEST(SocketTest, sendIgnoresEmptyBuffer)
{
    const auto result = socket->send({ });
    CHECK_EQUAL(0, result);
}

TEST(SocketTest, sendLimitsBufferSize)
{
    constexpr std::uint16_t maxSendSize = 2048;
    expectWaitForFreeRxTx(Mode::send, socketHandle, maxSendSize);
    mock("Device").expectOneCall("sendData").ignoreOtherParameters();
    mock("Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();

    auto buffer = createBuffer(maxSendSize + 1);
    const auto result = socket->send(buffer);
    CHECK_EQUAL(maxSendSize, result);
}

TEST(SocketTest, sendChecksFreesizeAndStatusFlagIfEstablished)
{
    constexpr std::uint16_t freeSize = defaultSize + 2;
    expectWaitForFreeRxTx(Mode::send, socketHandle, freeSize);
    mock("Device").expectOneCall("sendData").ignoreOtherParameters();
    mock("Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();

    auto buffer = createBuffer(defaultSize);
    const auto result = socket->send(buffer);
    CHECK_EQUAL(buffer.size(), result);
}

TEST(SocketTest, sendChecksFreesizeAndStatusFlagIfCloseWait)
{
    constexpr std::uint16_t freeSize = defaultSize + 2;
    expectWaitForFreeRxTx(Mode::send, socketHandle, freeSize);
    mock("Device").expectOneCall("sendData").ignoreOtherParameters();
    mock("Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();

    auto buffer = createBuffer(defaultSize);
    const auto result = socket->send(buffer);
    CHECK_EQUAL(buffer.size(), result);
}

TEST(SocketTest, sendChecksFreesizeAndStatusFlagNotEnoughFreeMemory)
{
    constexpr std::uint16_t freeSize = defaultSize;
    expectWaitForFreeRxTx(Mode::send, socketHandle, freeSize - 2);
    expectWaitForFreeRxTx(Mode::send, socketHandle, freeSize);
    mock("Device").expectOneCall("sendData").ignoreOtherParameters();
    mock("Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();

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
    expectWaitForFreeRxTx(Mode::send, socketHandle, static_cast<std::uint16_t>(buffer.size()));
    mock("Device").expectOneCall("sendData")
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
    expectWaitForFreeRxTx(Mode::send, socketHandle, static_cast<std::uint16_t>(buffer.size()));
    mock("Device").expectOneCall("sendData").ignoreOtherParameters();
    mock("Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();

    const auto result = socket->send(buffer);
    CHECK_EQUAL(buffer.size(), result);
}

TEST(SocketTest, sendSetsOkAfterSend)
{
    auto buffer = createBuffer(defaultSize);
    expectWaitForFreeRxTx(Mode::send, socketHandle, static_cast<std::uint16_t>(buffer.size()));
    mock("Device").expectOneCall("sendData").ignoreOtherParameters();
    mock("Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();

    const auto result = socket->send(buffer);
    CHECK_EQUAL(buffer.size(), result);
}

TEST(SocketTest, receiveReturnsBytesReceived)
{
    auto buffer = createBuffer(defaultSize);
    expectWaitForFreeRxTx(Mode::receive, socketHandle, 100);
    mock("Device").expectOneCall("receiveData")
        .ignoreOtherParameters()
        .andReturnValue(static_cast<std::uint16_t>(buffer.size()));
    mock("Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();

    std::array<std::uint8_t, defaultSize> data{};
    const auto result = socket->receive(data);
    CHECK_EQUAL(buffer.size(), result);
}

TEST(SocketTest, receiveIgnoresEmptyBuffer)
{
    std::vector<std::uint8_t> buffer;
    const auto result = socket->receive(buffer);
    CHECK_EQUAL(0, result);
}

TEST(SocketTest, receiveLimitsBuffer)
{
    constexpr std::uint16_t maxReceiveSize = 2048;
    std::array<std::uint8_t, maxReceiveSize + 1> data{};
    expectWaitForFreeRxTx(Mode::receive, socketHandle, maxReceiveSize);
    mock("Device").expectOneCall("receiveData")
        .ignoreOtherParameters()
        .andReturnValue(maxReceiveSize);
    mock("Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();

    const auto result = socket->receive(data);
    CHECK_EQUAL(maxReceiveSize, result);
}

TEST(SocketTest, receiveLimitsBufferToReceiveFreeSize)
{
    constexpr std::uint16_t size = defaultSize - 2;
    std::array<std::uint8_t, defaultSize> data{};
    expectWaitForFreeRxTx(Mode::receive, socketHandle, size);
    mock("Device").expectOneCall("receiveData")
        .ignoreOtherParameters()
        .andReturnValue(size);
    mock("Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();

    const auto result = socket->receive(data);
    CHECK_EQUAL(size, result);
}

TEST(SocketTest, receiveChecksStatusFlagIfEstablished)
{
    auto buffer = createBuffer(defaultSize);
    expectWaitForFreeRxTx(Mode::receive, socketHandle, 100);
    mock("Device").expectOneCall("receiveData")
        .ignoreOtherParameters()
        .andReturnValue(defaultSize);
    mock("Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();

    std::array<std::uint8_t, defaultSize> data{};
    const auto result = socket->receive(data);
    CHECK_EQUAL(buffer.size(), result);
}

TEST(SocketTest, receiveChecksStatusFlagIfCloseWait)
{
    auto buffer = createBuffer(defaultSize);
    expectWaitForFreeRxTx(Mode::receive, socketHandle, 100);
    mock("Device").expectOneCall("receiveData")
        .ignoreOtherParameters()
        .andReturnValue(defaultSize);
    mock("Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();

    std::array<std::uint8_t, defaultSize> data{};
    const auto result = socket->receive(data);
    CHECK_EQUAL(buffer.size(), result);
}

TEST(SocketTest, receiveReturnsErrorIfStatusNotEstablished)
{
    auto buffer = createBuffer(defaultSize);
    expectSocketStatusRead(socketHandle, SocketStatus::init);

    std::array<std::uint8_t, defaultSize> data{};
    const auto result = socket->receive(data);
    CHECK_EQUAL(0, result);
}

TEST(SocketTest, receiveReceivesData)
{
    auto buffer = createBuffer(defaultSize);
    gsl::span<std::uint8_t> bufferSpan(buffer);
    expectWaitForFreeRxTx(Mode::receive, socketHandle, 100);
    mock("Device").expectOneCall("receiveData")
        .withParameter("socket", socketHandle.value())
        .withOutputParameterReturning("buffer", buffer.data(), buffer.size())
        .withParameter("size", buffer.size())
        .andReturnValue(defaultSize);
    mock("Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();

    std::array<std::uint8_t, defaultSize> data{};
    const auto result = socket->receive(data);
    CHECK_EQUAL(buffer.size(), result);
}

TEST(SocketTest, receiveSendsCommandAfterReceive)
{
    auto buffer = createBuffer(defaultSize);
    gsl::span<std::uint8_t> bufferSpan(buffer);
    expectWaitForFreeRxTx(Mode::receive, socketHandle, 100);
    mock("Device").expectOneCall("receiveData")
        .ignoreOtherParameters()
        .andReturnValue(defaultSize);
    expectSocketCommand(socketHandle, SocketCommand::receive);

    std::array<std::uint8_t, defaultSize> data{};
    const auto result = socket->receive(data);
    CHECK_EQUAL(buffer.size(), result);
}

TEST(SocketTest, receiveWaitsForStatusFlagAfterSend)
{
    auto buffer = createBuffer(defaultSize);
    expectWaitForFreeRxTx(Mode::receive, socketHandle, 100);
    mock("Device").expectOneCall("receiveData")
        .ignoreOtherParameters()
        .andReturnValue(defaultSize);
    mock("Device").expectOneCall("executeSocketCommand")
        .ignoreOtherParameters();

    std::array<std::uint8_t, defaultSize> data{};
    const auto result = socket->receive(data);
    CHECK_EQUAL(buffer.size(), result);
}

TEST(SocketTest, receiveWaitsForDataAvailable)
{
    auto buffer = createBuffer(defaultSize);
    expectWaitForFreeRxTx(Mode::receive, socketHandle, 0);
    expectWaitForFreeRxTx(Mode::receive, socketHandle, defaultSize);

    mock("Device").expectOneCall("receiveData")
        .withParameter("socket", socketHandle.value())
        .withParameter("size", defaultSize)
        .ignoreOtherParameters()
        .andReturnValue(defaultSize);
    expectSocketCommand(socketHandle, SocketCommand::receive);

    std::array<std::uint8_t, defaultSize> data{};
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
    NetAddress<4> addr{{127, 0, 0, 1}};
    constexpr std::uint16_t portValue{4567};

    mock("Device").expectOneCall("setDestAddress")
                .withParameter("socket", socketHandle.value())
                .withMemoryBufferParameter("buffer", addr.data(), addr.size())
                .withParameter("port", portValue);
    expectSocketCommand(socketHandle, SocketCommand::connect);
    expectSocketStatusRead(socketHandle, SocketStatus::established);

    const auto rtn = socket->connect(addr, portValue);
    CHECK_EQUAL(Socket::Status::ok, rtn);
}

TEST(SocketTest, connectWaitsForEstablishedStatus)
{
    NetAddress<4> addr{{127, 0, 0, 1}};

    mock("Device").expectOneCall("setDestAddress").ignoreOtherParameters();
    mock("Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();
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
    NetAddress<4> addr{{127, 0, 0, 1}};

    mock("Device").expectOneCall("setDestAddress").ignoreOtherParameters();
    mock("Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();
    expectSocketStatusRead(socketHandle, SocketStatus::init);
    expectSocketStatusRead(socketHandle, SocketStatus::closed);

    const auto rtn = socket->connect(addr, 4567);
    CHECK_EQUAL(Socket::Status::closed, rtn);
}

TEST(SocketTest, connectErrorOnTimeout)
{
    NetAddress<4> addr{{127, 0, 0, 1}};

    mock("Device").expectOneCall("setDestAddress").ignoreOtherParameters();
    mock("Device").expectOneCall("executeSocketCommand").ignoreOtherParameters();
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

