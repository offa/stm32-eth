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
#include "mock/W5100DeviceSpy.h"
#include <vector>
#include <algorithm>
#include <numeric>
#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>

using eth::SocketCommand;
using eth::SocketStatus;
using eth::Mode;

inline SimpleString StringFrom(eth::SocketStatus status)
{
    return SimpleString("0x") + HexStringFrom(static_cast<unsigned int>(status));
}

inline SimpleString StringFrom(eth::SocketCommand cmd)
{
    return SimpleString("0x") + HexStringFrom(static_cast<unsigned int>(cmd));
}


TEST_GROUP(W5100DeviceTest)
{
    void setup() override
    {
        device = std::make_unique<test::W5100DeviceSpy>();
        mock().strictOrder();
    }

    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
    }

    void expectWrite(uint16_t addr, uint8_t data) const
    {
        spiMock.expectOneCall("setSlaveSelect");
        spiMock.expectOneCall("transfer").withParameter("data", 0xf0);
        spiMock.expectOneCall("transfer").withParameter("data", addr >> 8);
        spiMock.expectOneCall("transfer").withParameter("data", addr & 0xff);
        spiMock.expectOneCall("transfer").withParameter("data", data);
        spiMock.expectOneCall("resetSlaveSelect");
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
            expectWrite(addr, value);
            ++addr;
        });
    }

    void expectRead(uint16_t addr, uint8_t data) const
    {
        spiMock.expectOneCall("setSlaveSelect");
        spiMock.expectOneCall("transfer").withParameter("data", 0x0f);
        spiMock.expectOneCall("transfer").withParameter("data", addr >> 8);
        spiMock.expectOneCall("transfer").withParameter("data", addr & 0xff);
        spiMock.expectOneCall("transfer").withParameter("data", 0).andReturnValue(data);
        spiMock.expectOneCall("resetSlaveSelect");
    }

    void expectRead(uint16_t addr, uint16_t data) const
    {
        expectRead(addr, static_cast<uint8_t>(data >> 8));
        expectRead(addr + 1, static_cast<uint8_t>(data & 0xff));
    }

    void checkWriteCalls(size_t expectedCalls) const
    {
        constexpr size_t transmissionsPerWrite = 4;
        auto actual = spiMock.getData("transfer::count").getUnsignedIntValue();
        CHECK_EQUAL(expectedCalls * transmissionsPerWrite, actual);
    }

    std::vector<uint8_t> createBuffer(size_t size) const
    {
        std::vector<uint8_t> buffer(size);
        std::iota(buffer.begin(), buffer.end(), 0);
        return buffer;
    }


    std::unique_ptr<test::W5100DeviceSpy> device;
    MockSupport& spiMock = mock("Spi");
    static constexpr eth::SocketHandle socket = 0;
    static constexpr uint16_t socktBaseAddr = 0x0400;
    static constexpr uint16_t socktChannelSize = 0x0100;
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

    device->init();
}

TEST(W5100DeviceTest, writeByte)
{
    constexpr uint16_t address = 0xaabb;
    constexpr uint8_t data = 0xef;
    expectWrite(address, data);

    device->write(address, data);
}

TEST(W5100DeviceTest, writeBuffer)
{
    constexpr uint16_t address = 0xa1b2;
    constexpr uint16_t size = 10;
    auto data = createBuffer(size);

    spiMock.expectNCalls(size, "setSlaveSelect");
    spiMock.ignoreOtherCalls();

    device->write(address, data.data(), data.size());
    checkWriteCalls(size);
}

TEST(W5100DeviceTest, readByte)
{
    constexpr uint16_t address = 0xccdd;
    constexpr uint8_t data = 0xef;
    expectRead(address, data);

    uint8_t result = device->read(address);
    CHECK_EQUAL(data, result);
}

TEST(W5100DeviceTest, writeSocketModeRegister)
{
    constexpr uint16_t addressOffset = 0x0000;
    constexpr uint16_t address = socktBaseAddr + socket * socktChannelSize + addressOffset;
    constexpr uint8_t value = 0x17;
    expectWrite(address, value);

    device->writeSocketModeRegister(socket, value);
}

TEST(W5100DeviceTest, writeSocketSourcePort)
{
    constexpr uint16_t addressOffset = 0x0004;
    constexpr uint16_t address = socktBaseAddr + socket * socktChannelSize + addressOffset;
    constexpr uint16_t value = 0x1234;
    expectWrite(address, value);

    device->writeSocketSourcePort(socket, value);
}

TEST(W5100DeviceTest, writeSocketInterruptRegister)
{
    constexpr uint16_t addressOffset = 0x0002;
    constexpr uint16_t address = socktBaseAddr + socket * socktChannelSize + addressOffset;
    constexpr uint8_t value = 0x45;
    expectWrite(address, value);

    device->writeSocketInterruptRegister(socket, value);
}

TEST(W5100DeviceTest, readSocketInterruptRegister)
{
    constexpr uint16_t addressOffset = 0x0002;
    constexpr uint16_t address = socktBaseAddr + socket * socktChannelSize + addressOffset;
    constexpr uint8_t value = 0xb8;
    expectRead(address, value);

    uint8_t rtn = device->readSocketInterruptRegister(socket);
    CHECK_EQUAL(value, rtn);
}

TEST(W5100DeviceTest, writeSocketCommandRegister)
{
    constexpr uint16_t addressOffset = 0x0001;
    constexpr uint16_t address = socktBaseAddr + socket * socktChannelSize + addressOffset;
    constexpr SocketCommand cmd = SocketCommand::listen;
    expectWrite(address, static_cast<uint8_t>(cmd));

    device->writeSocketCommandRegister(socket, cmd);
}

TEST(W5100DeviceTest, readSocketCommandRegister)
{
    constexpr uint16_t addressOffset = 0x0001;
    constexpr uint16_t address = socktBaseAddr + socket * socktChannelSize + addressOffset;
    constexpr SocketCommand cmd = SocketCommand::connect;
    expectRead(address, static_cast<uint8_t>(cmd));

    auto rtn = device->readSocketCommandRegister(socket);
    CHECK_EQUAL(cmd, rtn);
}

TEST(W5100DeviceTest, executeSocketCommand)
{
    constexpr SocketCommand cmd = SocketCommand::connect;
    constexpr uint8_t registerCleared = 0x00;
    constexpr uint16_t addressOffset = 0x0001;
    constexpr uint16_t address = socktBaseAddr + socket * socktChannelSize + addressOffset;
    expectWrite(address, static_cast<uint8_t>(cmd));
    expectRead(address, static_cast<uint8_t>(0x01));
    expectRead(address, static_cast<uint8_t>(0x01));
    expectRead(address, static_cast<uint8_t>(registerCleared));

    device->executeSocketCommand(socket, cmd);
}

TEST(W5100DeviceTest, readSocketStatusRegister)
{
    constexpr uint16_t addressOffset = 0x0003;
    constexpr uint16_t address = socktBaseAddr + socket * socktChannelSize + addressOffset;
    constexpr SocketStatus status = SocketStatus::established;
    expectRead(address, static_cast<uint8_t>(status));

    auto rtn = device->readSocketStatusRegister(socket);
    CHECK_EQUAL(status, rtn);
}

TEST(W5100DeviceTest, readSocketTransmitFreeSizeRegister)
{
    constexpr uint16_t addressOffset = 0x0020;
    constexpr uint16_t address = socktBaseAddr + socket * socktChannelSize + addressOffset;
    constexpr uint16_t value = 0x2345;
    expectRead(address, value);

    uint16_t rtn = device->spy_readSocketTransmitFreeSizeRegister(socket);
    CHECK_EQUAL(value, rtn);
}

TEST(W5100DeviceTest, getTransmitFreeSize)
{
    constexpr uint16_t addressOffset = 0x0020;
    constexpr uint16_t address = socktBaseAddr + socket * socktChannelSize + addressOffset;
    constexpr uint16_t value = 0x1234;
    expectRead(address, static_cast<uint16_t>(0xaaaa));
    expectRead(address, static_cast<uint16_t>(0xbbbb));
    expectRead(address, static_cast<uint16_t>(0x1234));
    expectRead(address, static_cast<uint16_t>(0x1234));

    uint16_t rtn = device->getTransmitFreeSize(socket);
    CHECK_EQUAL(value, rtn);
}

TEST(W5100DeviceTest, readSocketTransmitWritePointer)
{
    constexpr uint16_t addressOffset = 0x0024;
    constexpr uint16_t address = socktBaseAddr + socket * socktChannelSize + addressOffset;
    constexpr uint16_t value = 0xeebb;
    expectRead(address, value);

    uint16_t rtn = device->spy_readSocketTransmitWritePointer(socket);
    CHECK_EQUAL(value, rtn);
}

TEST(W5100DeviceTest, writeSocketTransmitWritePointer)
{
    constexpr uint16_t addressOffset = 0x0024;
    constexpr uint16_t address = socktBaseAddr + socket * socktChannelSize + addressOffset;
    constexpr uint16_t value = 0xabcd;
    expectWrite(address, value);

    device->spy_writeSocketTransmitWritePointer(socket, value);
}

TEST(W5100DeviceTest, sendData)
{
    constexpr uint16_t addressOffset = 0x0024;
    constexpr uint16_t address = socktBaseAddr + socket * socktChannelSize + addressOffset;
    constexpr uint16_t value = 0x3355;
    expectRead(address, value);

    constexpr uint16_t destAddress = 0x4355;
    constexpr uint16_t size = 5;
    auto buffer = createBuffer(size);
    expectWrite(destAddress, buffer);
    expectWrite(address, static_cast<uint16_t>(value + size));

    device->sendData(socket, buffer.data(), buffer.size());
}

TEST(W5100DeviceTest, sendDataCircularBufferWrap)
{
    const uint16_t size = device->getTransmitBufferSize() + 5;
    auto buffer = createBuffer(size);
    spiMock.ignoreOtherCalls();

    device->sendData(socket, buffer.data(), buffer.size());
    checkWriteCalls(sizeof(uint16_t) + size + sizeof(uint16_t));
}

TEST(W5100DeviceTest, setGatewayAddress)
{
    constexpr uint16_t address = 0x0001;
    std::array<uint8_t, 4> value = {{ 1, 2, 3, 4 }};
    expectWrite(address, value);

    device->setGatewayAddress(value);
}

TEST(W5100DeviceTest, setSubnetMask)
{
    constexpr uint16_t address = 0x0005;
    std::array<uint8_t, 4> value = {{ 1, 2, 3, 4 }};
    expectWrite(address, value);

    device->setSubnetMask(value);
}

TEST(W5100DeviceTest, setMacAddress)
{
    constexpr uint16_t address = 0x0009;
    std::array<uint8_t, 6> value = {{ 1, 2, 3, 4, 5, 6 }};
    expectWrite(address, value);

    device->setMacAddress(value);
}

TEST(W5100DeviceTest, setIpAddress)
{
    constexpr uint16_t address = 0x000f;
    std::array<uint8_t, 4> value = {{ 1, 2, 3, 4 }};
    expectWrite(address, value);

    device->setIpAddress(value);
}

TEST(W5100DeviceTest, writeTransmitMemorySizeRegister)
{
    constexpr uint16_t address = 0x001b;
    constexpr uint8_t value = 0xaa;
    expectWrite(address, value);

    device->spy_writeTransmitMemorySizeRegister(value);
}

TEST(W5100DeviceTest, writeReceiveMemorySizeRegister)
{
    constexpr uint16_t address = 0x001a;
    constexpr uint8_t value = 0xbb;
    expectWrite(address, value);

    device->spy_writeReceiveMemorySizeRegister(value);
}

TEST(W5100DeviceTest, writeModeRegister)
{
    constexpr uint16_t address = 0x0000;
    constexpr Mode mode = Mode::autoIncrement;
    expectWrite(address, static_cast<uint8_t>(mode));

    device->writeModeRegister(mode);
}

