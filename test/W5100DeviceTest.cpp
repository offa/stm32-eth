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
#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>


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
        mock("Spi").expectOneCall("setSlaveSelect");
        mock("Spi").expectOneCall("transfer").withParameter("data", 0xf0);
        mock("Spi").expectOneCall("transfer").withParameter("data", addr >> 8);
        mock("Spi").expectOneCall("transfer").withParameter("data", addr & 0xff);
        mock("Spi").expectOneCall("transfer").withParameter("data", data);
        mock("Spi").expectOneCall("resetSlaveSelect");
    }

    void expectWrite(uint16_t addr, uint16_t data) const
    {
        expectWrite(addr, static_cast<uint8_t>(data >> 8));
        expectWrite(addr + 1, static_cast<uint8_t>(data & 0xff));
    }

    void expectRead(uint16_t addr, uint8_t data) const
    {
        mock("Spi").expectOneCall("setSlaveSelect");
        mock("Spi").expectOneCall("transfer").withParameter("data", 0x0f);
        mock("Spi").expectOneCall("transfer").withParameter("data", addr >> 8);
        mock("Spi").expectOneCall("transfer").withParameter("data", addr & 0xff);
        mock("Spi").expectOneCall("transfer").withParameter("data", 0).andReturnValue(data);
        mock("Spi").expectOneCall("resetSlaveSelect");
    }

    void expectRead(uint16_t addr, uint16_t data) const
    {
        expectRead(addr, static_cast<uint8_t>(data >> 8));
        expectRead(addr + 1, static_cast<uint8_t>(data & 0xff));
    }


    std::unique_ptr<test::W5100DeviceSpy> device;
    static constexpr eth::SocketHandle socket = 0;
    static constexpr uint16_t CH_BASE = 0x0400;
    static constexpr uint16_t CH_SIZE = 0x0100;
};

TEST(W5100DeviceTest, initSetsResetBitAndMemorySize)
{
    constexpr uint16_t addrMR = 0x0000;
    constexpr uint8_t resetBit = 7;
    constexpr uint8_t valueReset = 1 << resetBit;
    expectWrite(addrMR, valueReset);

    constexpr uint16_t addrRxSize = 0x001a;
    constexpr uint16_t addrTxSize = 0x001b;
    constexpr uint8_t valueMemorySize = 0x55;
    expectWrite(addrTxSize, valueMemorySize);
    expectWrite(addrRxSize, valueMemorySize);

    device->init();
}

TEST(W5100DeviceTest, writeByte)
{
    constexpr uint16_t addr = 0xaabb;
    constexpr uint8_t data = 0xef;
    expectWrite(addr, data);

    device->write(addr, data);
}

TEST(W5100DeviceTest, writeBuffer)
{
    constexpr uint16_t addr = 0xa1b2;
    constexpr uint16_t size = 50;
    uint8_t data[size];
    for( uint16_t i=0; i<size; ++i )
    {
        data[i] = i;
        expectWrite(addr + i, data[i]);
    }

    device->write(addr, data, size);
}

TEST(W5100DeviceTest, readByte)
{
    constexpr uint16_t addr = 0xccdd;
    constexpr uint8_t data = 0xef;
    expectRead(addr, data);

    uint8_t result = device->read(addr);
    CHECK_EQUAL(data, result);
}

TEST(W5100DeviceTest, writeSocketModeRegister)
{
    constexpr uint16_t addr_ = 0x0000;
    constexpr uint16_t addr = CH_BASE + socket * CH_SIZE + addr_;
    constexpr uint8_t value = 0x17;
    expectWrite(addr, value);

    device->writeSocketModeRegister(socket, value);
}

TEST(W5100DeviceTest, writeSocketSourcePort)
{
    constexpr uint16_t addr_ = 0x0004;
    constexpr uint16_t addr = CH_BASE + socket * CH_SIZE + addr_;
    constexpr uint16_t value = 0x1234;
    expectWrite(addr, value);

    device->writeSocketSourcePort(socket, value);
}

TEST(W5100DeviceTest, writeSocketInterruptRegister)
{
    constexpr uint16_t addr_ = 0x0002;
    constexpr uint16_t addr = CH_BASE + socket * CH_SIZE + addr_;
    constexpr uint8_t value = 0x45;
    expectWrite(addr, value);

    device->writeSocketInterruptRegister(socket, value);
}

TEST(W5100DeviceTest, readSocketInterruptRegister)
{
    constexpr uint16_t addr_ = 0x0002;
    constexpr uint16_t addr = CH_BASE + socket * CH_SIZE + addr_;
    constexpr uint8_t value = 0xb8;
    expectRead(addr, value);

    uint8_t rtn = device->readSocketInterruptRegister(socket);
    CHECK_EQUAL(value, rtn);
}

TEST(W5100DeviceTest, writeSocketCommandRegister)
{
    constexpr uint16_t addr_ = 0x0001;
    constexpr uint16_t addr = CH_BASE + socket * CH_SIZE + addr_;
    constexpr uint8_t value = 0x5e;
    expectWrite(addr, value);

    device->writeSocketCommandRegister(socket, value);
}

TEST(W5100DeviceTest, readSocketCommandRegister)
{
    constexpr uint16_t addr_ = 0x0001;
    constexpr uint16_t addr = CH_BASE + socket * CH_SIZE + addr_;
    constexpr uint8_t value = 0xa9;
    expectRead(addr, value);

    uint8_t rtn = device->readSocketCommandRegister(socket);
    CHECK_EQUAL(value, rtn);
}

TEST(W5100DeviceTest, executeSocketCommand)
{
    constexpr uint8_t cmd = 0x01;
    constexpr uint16_t addr_ = 0x0001;
    constexpr uint16_t addr = CH_BASE + socket * CH_SIZE + addr_;
    expectWrite(addr, cmd);
    expectRead(addr, static_cast<uint8_t>(0x01));
    expectRead(addr, static_cast<uint8_t>(0x01));
    expectRead(addr, static_cast<uint8_t>(0x00));

    device->executeSocketCommand(socket, cmd);
}

TEST(W5100DeviceTest, readSocketStatusRegister)
{
    constexpr uint16_t addr_ = 0x0003;
    constexpr uint16_t addr = CH_BASE + socket * CH_SIZE + addr_;
    constexpr uint8_t value = 0x55;
    expectRead(addr, value);

    uint8_t rtn = device->readSocketStatusRegister(socket);
    CHECK_EQUAL(value, rtn);
}

TEST(W5100DeviceTest, readSocketTransmitFreeSizeRegister)
{
    constexpr uint16_t addr_ = 0x0020;
    constexpr uint16_t addr = CH_BASE + socket * CH_SIZE + addr_;
    constexpr uint16_t value = 0x2345;
    expectRead(addr, value);

    uint16_t rtn = device->spy_readSocketTransmitFreeSizeRegister(socket);
    CHECK_EQUAL(value, rtn);
}

TEST(W5100DeviceTest, getTransmitFreeSize)
{
    constexpr uint16_t addr_ = 0x0020;
    constexpr uint16_t addr = CH_BASE + socket * CH_SIZE + addr_;
    constexpr uint16_t value = 0x1234;
    expectRead(addr, static_cast<uint16_t>(0xaaaa));
    expectRead(addr, static_cast<uint16_t>(0xbbbb));
    expectRead(addr, static_cast<uint16_t>(0x1234));
    expectRead(addr, static_cast<uint16_t>(0x1234));

    uint16_t rtn = device->getTransmitFreeSize(socket);
    CHECK_EQUAL(value, rtn);
}

TEST(W5100DeviceTest, readSocketTransmitWritePointer)
{
    constexpr uint16_t addr_ = 0x0024;
    constexpr uint16_t addr = CH_BASE + socket * CH_SIZE + addr_;
    constexpr uint16_t value = 0xeebb;
    expectRead(addr, value);

    uint16_t rtn = device->spy_readSocketTransmitWritePointer(socket);
    CHECK_EQUAL(value, rtn);
}

TEST(W5100DeviceTest, writeSocketTransmitWritePointer)
{
    constexpr uint16_t addr_ = 0x0024;
    constexpr uint16_t addr = CH_BASE + socket * CH_SIZE + addr_;
    constexpr uint16_t value = 0xabcd;
    expectWrite(addr, value);

    device->spy_writeSocketTransmitWritePointer(socket, value);
}

TEST(W5100DeviceTest, sendData)
{
    // TODO: Test circular buffer if wraped around
    // Read write register pos
    constexpr uint16_t addr_ = 0x0024;
    constexpr uint16_t addr = CH_BASE + socket * CH_SIZE + addr_;
    constexpr uint16_t value = 0x3355;
    expectRead(addr, value);

    constexpr uint16_t destAddr = 0x4355;

    constexpr uint16_t size = 50;
    uint8_t buffer[size];

    for( uint16_t i=0; i<size; ++i )
    {
        buffer[i] = i;
        expectWrite(destAddr + i, buffer[i]);
    }
    expectWrite(addr, static_cast<uint16_t>(value + size));

    device->sendData(socket, buffer, size);
}

TEST(W5100DeviceTest, writeGatewayAddressRegister)
{
    constexpr uint16_t addr = 0x0001;
    constexpr uint8_t size = 4;
    uint8_t value[size];
    for( uint8_t i=0; i<size; ++i )
    {
        value[i] = i;
        expectWrite(addr + i , value[i]);
    }

    device->spy_writeGatewayAddressRegister(value);
}

TEST(W5100DeviceTest, writeSubnetMaskRegister)
{
    constexpr uint16_t addr = 0x0005;
    constexpr uint8_t size = 4;
    uint8_t value[size];
    for( uint8_t i=0; i<size; ++i )
    {
        value[i] = i;
        expectWrite(addr + i , value[i]);
    }

    device->spy_writeSubnetMaskRegister(value);
}

TEST(W5100DeviceTest, writeSourceMacAddressRegister)
{
    constexpr uint16_t addr = 0x0009;
    constexpr uint8_t size = 6;
    uint8_t value[size];
    for( uint8_t i=0; i<size; ++i )
    {
        value[i] = i;
        expectWrite(addr + i , value[i]);
    }

    device->spy_writeSourceMacAddressRegister(value);
}

TEST(W5100DeviceTest, writeSourceIpRegister)
{
    constexpr uint16_t addr = 0x000f;
    constexpr uint8_t size = 4;
    uint8_t value[size];
    for( uint8_t i=0; i<size; ++i )
    {
        value[i] = i;
        expectWrite(addr + i , value[i]);
    }

    device->spy_writeSourceIpRegister(value);
}

TEST(W5100DeviceTest, setGatewayAddress)
{
    constexpr uint16_t addr = 0x0001;
    constexpr uint8_t size = 4;
    uint8_t value[size];
    for( uint8_t i=0; i<size; ++i )
    {
        value[i] = i;
        expectWrite(addr + i , value[i]);
    }

    device->setGatewayAddress(value);
}

TEST(W5100DeviceTest, setSubnetMask)
{
    constexpr uint16_t addr = 0x0005;
    constexpr uint8_t size = 4;
    uint8_t value[size];
    for( uint8_t i=0; i<size; ++i )
    {
        value[i] = i;
        expectWrite(addr + i , value[i]);
    }

    device->setSubnetMask(value);
}

TEST(W5100DeviceTest, setMacAddress)
{
    constexpr uint16_t addr = 0x0009;
    constexpr uint8_t size = 6;
    uint8_t value[size];
    for( uint8_t i=0; i<size; ++i )
    {
        value[i] = i;
        expectWrite(addr + i , value[i]);
    }

    device->setMacAddress(value);
}

TEST(W5100DeviceTest, setIpAddress)
{
    constexpr uint16_t addr = 0x000f;
    constexpr uint8_t size = 4;
    uint8_t value[size];
    for( uint8_t i=0; i<size; ++i )
    {
        value[i] = i;
        expectWrite(addr + i , value[i]);
    }

    device->setIpAddress(value);
}

TEST(W5100DeviceTest, writeTransmitMemorySizeRegister)
{
    constexpr uint16_t addr = 0x001b;
    constexpr uint8_t value = 0xaa;
    expectWrite(addr, value);

    device->spy_writeTransmitMemorySizeRegister(value);
}

TEST(W5100DeviceTest, writeReceiveMemorySizeRegister)
{
    constexpr uint16_t addr = 0x001a;
    constexpr uint8_t value = 0xbb;
    expectWrite(addr, value);

    device->spy_writeReceiveMemorySizeRegister(value);
}

TEST(W5100DeviceTest, writeModeRegister)
{
    constexpr uint16_t addr = 0x0000;
    constexpr uint8_t value = 0x07;
    expectWrite(addr, value);

    device->writeModeRegister(value);
}

