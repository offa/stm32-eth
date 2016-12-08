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
#include "SpiWriter.h"
#include "TestHelper.h"
#include <vector>
#include <algorithm>
#include <numeric>
#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>

using eth::W5100Device;
using eth::SpiWriter;
using eth::SocketHandle;
using eth::SocketCommand;
using eth::SocketStatus;
using eth::Mode;
using eth::W5100Register;
using eth::SocketInterrupt;
using eth::makeRegister;


namespace
{
    constexpr SocketHandle socketHandle = eth::makeHandle<0>();
}


TEST_GROUP(W5100DeviceTest)
{
    void setup() override
    {
        mock().strictOrder();

        auto f = gsl::finally([this]
        {
            mock().enable();
            writerMock.setData("write::count", 0);
        });

        mock().disable();

        device = std::make_unique<W5100Device>(writer);
    }

    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
    }

    void expectWrite(uint16_t addr, uint8_t data) const
    {
        writerMock.expectOneCall("write")
            .withParameter("address", addr)
            .withParameter("data", data);
    }

    void expectWrite(uint16_t addr, uint16_t data) const
    {
        expectWrite(addr, static_cast<uint8_t>(data >> 8));
        expectWrite(addr + 1, static_cast<uint8_t>(data & 0xff));
    }

    template<class Container>
    void expectWrite(uint16_t addr, const Container& data) const
    {
        std::for_each(data.begin(), data.end(), [&](uint8_t value)
        {
            expectWrite(addr++, value);
        });
    }

    void checkWriteCalls(size_t expectedCalls) const
    {
        const auto actual = writerMock.getData("write::count").getUnsignedIntValue();
        CHECK_EQUAL(expectedCalls, actual);
    }

    void checkReadCalls(size_t expectedCalls) const
    {
        const auto actual = writerMock.getData("read::count").getUnsignedIntValue();
        CHECK_EQUAL(expectedCalls, actual);
    }

    void expectRead(uint16_t addr, uint8_t data) const
    {
        writerMock.expectOneCall("read")
            .withParameter("address", addr)
            .andReturnValue(data);
    }

    void expectRead(uint16_t addr, uint16_t data) const
    {
        expectRead(addr, static_cast<uint8_t>(data >> 8));
        expectRead(addr + 1, static_cast<uint8_t>(data & 0xff));
    }

    template<class Container>
    void expectRead(uint16_t addr, const Container& data) const
    {
        std::for_each(data.begin(), data.end(), [&](uint8_t value)
        {
            expectRead(addr++, value);
        });
    }

    std::vector<uint8_t> createBuffer(size_t size) const
    {
        std::vector<uint8_t> buffer(size);
        std::iota(buffer.begin(), buffer.end(), 0);
        return buffer;
    }

    template<class T>
    W5100Register<T> asRegister(uint16_t address, T) const
    {
        return makeRegister<T>(address);
    }

    static constexpr uint16_t toAddress(eth::SocketHandle s, uint16_t address)
    {
        return eth::makeRegister<uint8_t>(s, address).address();
    }


    std::unique_ptr<W5100Device> device;
    SpiWriter writer;
    MockSupport& writerMock = mock("SpiWriter");
};

TEST(W5100DeviceTest, initSetsResetBitAndMemorySize)
{
    constexpr uint16_t addressModeReg = 0x0000;
    constexpr uint8_t resetBit = 7;
    constexpr uint8_t valueReset = 1 << resetBit;
    expectWrite(addressModeReg, valueReset);

    constexpr uint16_t addressRxSize = 0x001a;
    constexpr uint16_t addressTxSize = 0x001b;
    constexpr uint8_t valueMemorySize = 0x55;
    expectWrite(addressTxSize, valueMemorySize);
    expectWrite(addressRxSize, valueMemorySize);

    W5100Device d(writer);
}

TEST(W5100DeviceTest, writeRegisterByte)
{
    constexpr auto reg = makeRegister<uint8_t>(0xabcd);
    constexpr uint8_t data = 0xef;
    expectWrite(0xabcd, data);

    device->write(reg, data);
}

TEST(W5100DeviceTest, writeRegisterTwoByte)
{
    constexpr auto reg = makeRegister<uint16_t>(0xabcd);
    constexpr uint16_t data = 0x0011;
    expectWrite(0xabcd, data);

    device->write(reg, data);
}

TEST(W5100DeviceTest, writeSpan)
{
    constexpr uint16_t size = 10;
    const auto data = createBuffer(size);
    auto span = gsl::make_span(data);
    const auto reg = asRegister(0xa1b2, span);

    expectWrite(0xa1b2, span);
    device->write(reg, span);
}

TEST(W5100DeviceTest, writeBuffer)
{
    constexpr uint16_t size = 10;
    const auto data = createBuffer(size);
    const auto reg = asRegister(0xa1b2, data);

    writerMock.ignoreOtherCalls();

    device->write(reg, data);
    checkWriteCalls(size);
}

TEST(W5100DeviceTest, writeBufferByPointerAndSize)
{
    constexpr uint16_t size = 10;
    const auto data = createBuffer(size);
    auto span = gsl::make_span(data);
    auto reg = asRegister(0xa1b2, span);

    writerMock.ignoreOtherCalls();

    device->write(reg, {data.data(), size});
    checkWriteCalls(size);
}

TEST(W5100DeviceTest, readRegisterByte)
{
    constexpr auto reg = makeRegister<uint8_t>(0xddee);
    constexpr uint8_t data = 0xef;
    expectRead(0xddee, data);

    const uint8_t result = device->read(reg);
    CHECK_EQUAL(data, result);
}

TEST(W5100DeviceTest, readRegisterTwoByte)
{
    constexpr auto reg = makeRegister<uint16_t>(0xddee);
    constexpr uint16_t data = 0xef01;
    expectRead(0xddee, data);

    const uint16_t result = device->readWord(reg);
    CHECK_EQUAL(data, result);
}

TEST(W5100DeviceTest, readRegisterSpan)
{
    constexpr uint16_t size = 10;
    const auto data = createBuffer(size);
    expectRead(0xddee, data);

    std::array<uint8_t, size> buffer;
    const auto reg = asRegister(0xddee, buffer);

    const auto result = device->read(reg, buffer);
    CHECK_EQUAL(size, result);
    CHECK_TRUE(std::equal(data.begin(), data.end(), buffer.begin()));
}

TEST(W5100DeviceTest, writeSocketModeRegister)
{
    constexpr uint16_t address = toAddress(socketHandle, 0x0000);
    constexpr uint8_t value = 0x17;
    expectWrite(address, value);

    device->writeSocketModeRegister(socketHandle, value);
}

TEST(W5100DeviceTest, writeSocketSourcePort)
{
    constexpr uint16_t address = toAddress(socketHandle, 0x0004);
    constexpr uint16_t value = 0x1388;
    expectWrite(address, value);

    device->writeSocketSourcePort(socketHandle, value);
}

TEST(W5100DeviceTest, writeSocketInterruptRegister)
{
    constexpr uint16_t address = toAddress(socketHandle, 0x0002);
    constexpr uint8_t value = 0x45;
    expectWrite(address, value);

    device->writeSocketInterruptRegister(socketHandle, SocketInterrupt(value));
}

TEST(W5100DeviceTest, readSocketInterruptRegister)
{
    constexpr uint16_t address = toAddress(socketHandle, 0x0002);
    constexpr uint8_t value = 0xb8;
    expectRead(address, value);

    const auto rtn = device->readSocketInterruptRegister(socketHandle);
    CHECK_EQUAL(value, rtn.value());
}

TEST(W5100DeviceTest, writeSocketCommandRegister)
{
    constexpr uint16_t address = toAddress(socketHandle, 0x0001);
    constexpr SocketCommand cmd = SocketCommand::listen;
    expectWrite(address, static_cast<uint8_t>(cmd));

    device->writeSocketCommandRegister(socketHandle, cmd);
}

TEST(W5100DeviceTest, readSocketCommandRegister)
{
    constexpr uint16_t address = toAddress(socketHandle, 0x0001);
    constexpr SocketCommand cmd = SocketCommand::connect;
    expectRead(address, static_cast<uint8_t>(cmd));

    const auto rtn = device->readSocketCommandRegister(socketHandle);
    CHECK_EQUAL(cmd, rtn);
}

TEST(W5100DeviceTest, executeSocketCommand)
{
    constexpr SocketCommand cmd = SocketCommand::connect;
    constexpr uint8_t registerCleared = 0x00;
    constexpr uint16_t address = toAddress(socketHandle, 0x0001);
    expectWrite(address, static_cast<uint8_t>(cmd));
    expectRead(address, static_cast<uint8_t>(0x01));
    expectRead(address, static_cast<uint8_t>(0x01));
    expectRead(address, static_cast<uint8_t>(registerCleared));

    device->executeSocketCommand(socketHandle, cmd);
}

TEST(W5100DeviceTest, readSocketStatusRegister)
{
    constexpr uint16_t address = toAddress(socketHandle, 0x0003);
    constexpr SocketStatus status = SocketStatus::established;
    expectRead(address, static_cast<uint8_t>(status));

    const auto rtn = device->readSocketStatusRegister(socketHandle);
    CHECK_EQUAL(status, rtn);
}

TEST(W5100DeviceTest, getTransmitFreeSize)
{
    constexpr uint16_t address = toAddress(socketHandle, 0x0020);
    constexpr uint16_t value = 0x1234;
    expectRead(address, static_cast<uint16_t>(0xaaaa));
    expectRead(address, static_cast<uint16_t>(0xbbbb));
    expectRead(address, static_cast<uint16_t>(0x1234));
    expectRead(address, static_cast<uint16_t>(0x1234));

    const uint16_t rtn = device->getTransmitFreeSize(socketHandle);
    CHECK_EQUAL(value, rtn);
}

TEST(W5100DeviceTest, getReceiveFreeSize)
{
    constexpr uint16_t address = toAddress(socketHandle, 0x0026);
    constexpr uint16_t value = 0x1234;
    expectRead(address, static_cast<uint16_t>(0xaaaa));
    expectRead(address, static_cast<uint16_t>(0xbbbb));
    expectRead(address, static_cast<uint16_t>(0x1234));
    expectRead(address, static_cast<uint16_t>(0x1234));

    const uint16_t rtn = device->getReceiveFreeSize(socketHandle);
    CHECK_EQUAL(value, rtn);
}

TEST(W5100DeviceTest, sendData)
{
    constexpr uint16_t address = toAddress(socketHandle, 0x0024);
    constexpr uint16_t value = 0x3355;
    expectRead(address, value);

    constexpr uint16_t destAddress = 0x4355;
    constexpr uint16_t size = 5;
    auto buffer = createBuffer(size);
    expectWrite(destAddress, buffer);
    expectWrite(address, static_cast<uint16_t>(value + size));

    device->sendData(socketHandle, buffer);
}

TEST(W5100DeviceTest, sendDataCircularBufferWrap)
{
    constexpr auto ptrWrites = sizeof(uint16_t);
    const uint16_t size = device->getTransmitBufferSize() + 2;
    auto buffer = createBuffer(size);
    writerMock.ignoreOtherCalls();

    device->sendData(socketHandle, buffer);
    checkWriteCalls(size + ptrWrites);
}

TEST(W5100DeviceTest, receiveData)
{
    constexpr uint16_t address = toAddress(socketHandle, 0x0028);
    constexpr uint16_t value = 0x3355;
    expectRead(address, value);

    constexpr uint16_t destAddress = 0x6355;
    constexpr uint16_t size = 4;
    auto buffer = createBuffer(size);
    expectRead(destAddress, buffer);
    expectWrite(address, static_cast<uint16_t>(value + size));

    std::array<uint8_t, size> data;
    const auto rtn = device->receiveData(socketHandle, data);
    CHECK_EQUAL(size, rtn);
    CHECK_TRUE(std::equal(buffer.begin(), buffer.end(), data.begin()));
}

TEST(W5100DeviceTest, receiveDataCircularBufferWrap)
{
    constexpr auto ptrReads = sizeof(uint16_t);
    constexpr uint16_t size = eth::W5100Device::getReceiveBufferSize() + 2;
    auto buffer = createBuffer(size);
    writerMock.ignoreOtherCalls();

    std::array<uint8_t, size> data;
    const auto rtn = device->receiveData(socketHandle, data);
    CHECK_EQUAL(size, rtn);
    checkReadCalls(size + ptrReads);
}

TEST(W5100DeviceTest, setGatewayAddress)
{
    constexpr uint16_t address = 0x0001;
    std::array<uint8_t, 4> value = {{ 192, 168, 0, 1 }};
    expectWrite(address, value);

    device->setGatewayAddress(value);
}

TEST(W5100DeviceTest, setSubnetMask)
{
    constexpr uint16_t address = 0x0005;
    std::array<uint8_t, 4> value = {{ 255, 255, 255, 0 }};
    expectWrite(address, value);

    device->setSubnetMask(value);
}

TEST(W5100DeviceTest, setMacAddress)
{
    constexpr uint16_t address = 0x0009;
    std::array<uint8_t, 6> value = {{ 0x00, 0x08, 0xdc, 0x01, 0x02, 0x03 }};
    expectWrite(address, value);

    device->setMacAddress(value);
}

TEST(W5100DeviceTest, setIpAddress)
{
    constexpr uint16_t address = 0x000f;
    std::array<uint8_t, 4> value = {{ 192, 168, 0, 3 }};
    expectWrite(address, value);

    device->setIpAddress(value);
}

TEST(W5100DeviceTest, writeModeRegister)
{
    constexpr uint16_t address = 0x0000;
    constexpr Mode mode = Mode::autoIncrement;
    expectWrite(address, static_cast<uint8_t>(mode));

    device->writeModeRegister(mode);
}

TEST(W5100DeviceTest, setDestIpAddress)
{
    constexpr uint16_t address = toAddress(socketHandle, 0x000c);
    std::array<uint8_t, 4> value = {{ 192, 168, 1, 4 }};
    expectWrite(address, value);

    device->setDestIpAddress(socketHandle, value);
}

TEST(W5100DeviceTest, setDestPort)
{
    constexpr uint16_t address = toAddress(socketHandle, 0x0010);
    constexpr uint16_t value = 1234;
    expectWrite(address, value);

    device->setDestPort(socketHandle, value);
}

