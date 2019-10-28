/*
 * Stm32 Eth - Ethernet connectivity for Stm32
 * Copyright (C) 2016-2019  offa
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

#include "w5100/Device.h"
#include "spi/SpiWriter.h"
#include "Byte.h"
#include "TestHelper.h"
#include <vector>
#include <algorithm>
#include <memory>
#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>

using eth::Mode;
using eth::SocketCommand;
using eth::SocketHandle;
using eth::SocketInterrupt;
using eth::SocketStatus;
using eth::spi::SpiWriter;
using eth::w5100::Device;
using eth::w5100::makeRegister;
using eth::w5100::Register;


namespace
{
    constexpr inline SocketHandle socketHandle = eth::makeHandle<0>();
}


TEST_GROUP(W5100DeviceTest)
{
    void setup() override
    {
        mock().strictOrder();

        auto f = gsl::finally([] {
            mock().enable();
            mock("SpiWriter").setData("write::count", 0);
        });

        mock().disable();

        device = std::make_unique<Device>(writer);
    }

    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
    }

    void expectWrite(std::uint16_t addr, std::uint8_t data) const
    {
        mock("SpiWriter").expectOneCall("write").withParameter("address", addr).withParameter("data", data);
    }

    void expectWrite(std::uint16_t addr, std::uint16_t data) const
    {
        expectWrite(addr, eth::byte::get<1>(data));
        expectWrite(addr + 1, eth::byte::get<0>(data));
    }

    template <class Container>
    void expectWrite(std::uint16_t addr, const Container& data) const
    {
        std::for_each(data.begin(), data.end(), [this, &addr](std::uint8_t value) { expectWrite(addr++, value); });
    }

    void checkWriteCalls(std::size_t expectedCalls) const
    {
        const auto actual = mock("SpiWriter").getData("write::count").getUnsignedIntValue();
        CHECK_EQUAL(expectedCalls, actual);
    }

    void checkReadCalls(std::size_t expectedCalls) const
    {
        const auto actual = mock("SpiWriter").getData("read::count").getUnsignedIntValue();
        CHECK_EQUAL(expectedCalls, actual);
    }

    void expectRead(std::uint16_t addr, std::uint8_t data) const
    {
        mock("SpiWriter").expectOneCall("read").withParameter("address", addr).andReturnValue(data);
    }

    void expectRead(std::uint16_t addr, std::uint16_t data) const
    {
        expectRead(addr, eth::byte::get<1>(data));
        expectRead(addr + 1, eth::byte::get<0>(data));
    }

    template <class Container>
    void expectRead(std::uint16_t addr, const Container& data) const
    {
        std::for_each(data.begin(), data.end(), [this, &addr](std::uint8_t value) { expectRead(addr++, value); });
    }

    static constexpr std::uint16_t toAddress(eth::SocketHandle s, std::uint16_t address)
    {
        return makeRegister<std::uint8_t>(s, address).address();
    }


    std::unique_ptr<Device> device;
    SpiWriter writer{eth::spi::spi2};
};

TEST(W5100DeviceTest, initSetsResetBitAndMemorySize)
{
    constexpr std::uint16_t addressModeReg{0x0000};
    constexpr std::uint8_t resetBit{7};
    constexpr std::uint8_t valueReset{1u << resetBit};
    expectWrite(addressModeReg, valueReset);

    constexpr std::uint16_t addressRxSize{0x001a};
    constexpr std::uint16_t addressTxSize{0x001b};
    constexpr std::uint8_t valueMemorySize{0x55};
    expectWrite(addressTxSize, valueMemorySize);
    expectWrite(addressRxSize, valueMemorySize);

    Device d(writer);
}

TEST(W5100DeviceTest, writeRegisterByte)
{
    constexpr auto reg = makeRegister<std::uint8_t>(0xabcd);
    constexpr std::uint8_t data{0xef};
    expectWrite(0xabcd, data);

    device->write(reg, data);
}

TEST(W5100DeviceTest, writeRegisterTwoByte)
{
    constexpr auto reg = makeRegister<std::uint16_t>(0xabcd);
    constexpr std::uint16_t data{0x0011};
    expectWrite(0xabcd, data);

    device->write(reg, data);
}

TEST(W5100DeviceTest, writeSpan)
{
    constexpr std::uint16_t size{10};
    const auto data = createBuffer(size);
    auto span = gsl::make_span(data);
    const auto reg = Register<decltype(span)>(0xa1b2);

    expectWrite(0xa1b2, span);
    device->write(reg, span.cbegin(), span.cend());
}

TEST(W5100DeviceTest, writeBuffer)
{
    constexpr std::uint16_t size{10};
    const auto data = createBuffer(size);
    const auto reg = Register<decltype(data)>(0xa1b2);

    expectWrite(reg.address(), data);

    device->write(reg, data.cbegin(), data.cend());
}

TEST(W5100DeviceTest, writeBufferByPointerAndSize)
{
    constexpr std::uint16_t size{10};
    const auto data = createBuffer(size);
    auto span = gsl::make_span(data);
    auto reg = Register<decltype(span)>(0xa1b2);

    expectWrite(reg.address(), data);

    device->write(reg, data.data(), std::next(data.data(), size));
}

TEST(W5100DeviceTest, readRegisterByte)
{
    constexpr auto reg = makeRegister<std::uint8_t>(0xddee);
    constexpr std::uint8_t data{0xef};
    expectRead(0xddee, data);

    const std::uint8_t result = device->read(reg);
    CHECK_EQUAL(data, result);
}

TEST(W5100DeviceTest, readRegisterTwoByte)
{
    constexpr auto reg = makeRegister<std::uint16_t>(0xddee);
    constexpr std::uint16_t data{0xef01};
    expectRead(0xddee, data);

    const std::uint16_t result = device->read(reg);
    CHECK_EQUAL(data, result);
}

TEST(W5100DeviceTest, readRegisterSpan)
{
    constexpr std::uint16_t size{10};
    const auto data = createBuffer(size);
    expectRead(0xddee, data);

    std::array<std::uint8_t, size> buffer{};
    const auto reg = Register<decltype(buffer)>(0xddee);

    const auto result = device->read(reg, buffer.begin(), buffer.end());
    CHECK_EQUAL(size, result);
    CHECK_TRUE(std::equal(data.begin(), data.end(), buffer.begin()));
}

TEST(W5100DeviceTest, writeSocketModeRegister)
{
    constexpr std::uint16_t address = toAddress(socketHandle, 0x0000);
    constexpr std::uint8_t value{0x17};
    expectWrite(address, value);

    device->writeSocketModeRegister(socketHandle, value);
}

TEST(W5100DeviceTest, writeSocketSourcePort)
{
    constexpr std::uint16_t address = toAddress(socketHandle, 0x0004);
    constexpr std::uint16_t value{0x1388};
    expectWrite(address, value);

    device->writeSocketSourcePort(socketHandle, value);
}

TEST(W5100DeviceTest, writeSocketInterruptRegister)
{
    constexpr std::uint16_t address = toAddress(socketHandle, 0x0002);
    constexpr std::uint8_t value{0x45};
    expectWrite(address, value);

    device->writeSocketInterruptRegister(socketHandle, SocketInterrupt(value));
}

TEST(W5100DeviceTest, readSocketInterruptRegister)
{
    constexpr std::uint16_t address = toAddress(socketHandle, 0x0002);
    constexpr std::uint8_t value{0xb8};
    expectRead(address, value);

    const auto rtn = device->readSocketInterruptRegister(socketHandle);
    CHECK_EQUAL(value, rtn.value());
}

TEST(W5100DeviceTest, writeSocketCommandRegister)
{
    constexpr std::uint16_t address = toAddress(socketHandle, 0x0001);
    constexpr SocketCommand cmd = SocketCommand::listen;
    expectWrite(address, static_cast<std::uint8_t>(cmd));

    device->writeSocketCommandRegister(socketHandle, cmd);
}

TEST(W5100DeviceTest, readSocketCommandRegister)
{
    constexpr std::uint16_t address = toAddress(socketHandle, 0x0001);
    constexpr SocketCommand cmd = SocketCommand::connect;
    expectRead(address, static_cast<std::uint8_t>(cmd));

    const auto rtn = device->readSocketCommandRegister(socketHandle);
    CHECK_EQUAL(cmd, rtn);
}

TEST(W5100DeviceTest, executeSocketCommand)
{
    constexpr SocketCommand cmd = SocketCommand::connect;
    constexpr std::uint8_t registerCleared{0x00};
    constexpr std::uint16_t address = toAddress(socketHandle, 0x0001);
    expectWrite(address, static_cast<std::uint8_t>(cmd));
    expectRead(address, std::uint8_t{0x01});
    expectRead(address, std::uint8_t{0x01});
    expectRead(address, std::uint8_t{registerCleared});

    device->executeSocketCommand(socketHandle, cmd);
}

TEST(W5100DeviceTest, readSocketStatusRegister)
{
    constexpr std::uint16_t address = toAddress(socketHandle, 0x0003);
    constexpr SocketStatus status = SocketStatus::established;
    expectRead(address, static_cast<std::uint8_t>(status));

    const auto rtn = device->readSocketStatusRegister(socketHandle);
    CHECK_EQUAL(status, rtn);
}

TEST(W5100DeviceTest, getTransmitFreeSize)
{
    constexpr std::uint16_t address = toAddress(socketHandle, 0x0020);
    constexpr std::uint16_t value{0x1234};
    expectRead(address, std::uint16_t{0xaaaa});
    expectRead(address, std::uint16_t{0xbbbb});
    expectRead(address, std::uint16_t{0x1234});
    expectRead(address, std::uint16_t{0x1234});

    const std::uint16_t rtn = device->getTransmitFreeSize(socketHandle);
    CHECK_EQUAL(value, rtn);
}

TEST(W5100DeviceTest, getReceiveFreeSize)
{
    constexpr std::uint16_t address = toAddress(socketHandle, 0x0026);
    constexpr std::uint16_t value{0x1234};
    expectRead(address, std::uint16_t{0xaaaa});
    expectRead(address, std::uint16_t{0xbbbb});
    expectRead(address, std::uint16_t{0x1234});
    expectRead(address, std::uint16_t{0x1234});

    const std::uint16_t rtn = device->getReceiveFreeSize(socketHandle);
    CHECK_EQUAL(value, rtn);
}

TEST(W5100DeviceTest, sendData)
{
    constexpr std::uint16_t address = toAddress(socketHandle, 0x0024);
    constexpr std::uint16_t value{0x3355};
    expectRead(address, value);

    constexpr std::uint16_t destAddress{0x4355};
    constexpr std::uint16_t size{5};
    auto buffer = createBuffer(size);
    expectWrite(destAddress, buffer);
    expectWrite(address, std::uint16_t{value + size});

    device->sendData(socketHandle, buffer);
}

TEST(W5100DeviceTest, sendDataCircularBufferWrap)
{
    constexpr auto ptrWrites = sizeof(std::uint16_t);
    const std::uint16_t size = Device::getRxTxBufferSize() + 2;
    auto buffer = createBuffer(size);
    mock("SpiWriter").ignoreOtherCalls();

    device->sendData(socketHandle, buffer);
    checkWriteCalls(size + ptrWrites);
}

TEST(W5100DeviceTest, receiveData)
{
    constexpr std::uint16_t address = toAddress(socketHandle, 0x0028);
    constexpr std::uint16_t value{0x3355};
    expectRead(address, value);

    constexpr std::uint16_t destAddress{0x6355};
    constexpr std::uint16_t size{4};
    auto buffer = createBuffer(size);
    expectRead(destAddress, buffer);
    expectWrite(address, std::uint16_t{value + size});

    std::array<std::uint8_t, size> data{};
    const auto rtn = device->receiveData(socketHandle, data);
    CHECK_EQUAL(size, rtn);
    CHECK_TRUE(std::equal(buffer.begin(), buffer.end(), data.begin()));
}

TEST(W5100DeviceTest, receiveDataCircularBufferWrap)
{
    constexpr auto ptrReads = sizeof(std::uint16_t);
    constexpr std::uint16_t size = Device::getRxTxBufferSize() + 2;
    mock("SpiWriter").ignoreOtherCalls();

    std::array<std::uint8_t, size> data{};
    const auto rtn = device->receiveData(socketHandle, data);
    CHECK_EQUAL(size, rtn);
    checkReadCalls(size + ptrReads);
}

TEST(W5100DeviceTest, writeModeRegister)
{
    constexpr std::uint16_t address{0x0000};
    constexpr Mode mode = Mode::autoIncrement;
    expectWrite(address, static_cast<std::uint8_t>(mode));

    device->writeModeRegister(mode);
}

TEST(W5100DeviceTest, setDestAddress)
{
    constexpr std::uint16_t addressIp = toAddress(socketHandle, 0x000c);
    constexpr std::uint16_t addressPort = toAddress(socketHandle, 0x0010);
    eth::NetAddress<4> ip{{192, 168, 1, 4}};
    constexpr std::uint16_t port{1234};
    expectWrite(addressIp, ip);
    expectWrite(addressPort, port);

    device->setDestAddress(socketHandle, ip, port);
}

TEST(W5100DeviceTest, configureNetConfiguration)
{
    constexpr eth::NetConfig config{
        {{192, 168, 0, 3}}, {{255, 255, 255, 0}}, {{192, 168, 0, 1}}, {{0x00, 0x08, 0xdc, 0x01, 0x02, 0x03}}};

    const auto [ip, netmask, gateway, mac] = config;

    constexpr std::uint16_t addrIp{0x000f};
    constexpr std::uint16_t addrNetmask{0x0005};
    constexpr std::uint16_t addrGateway{0x0001};
    constexpr std::uint16_t addrMac{0x0009};

    expectWrite(addrIp, ip);
    expectWrite(addrNetmask, netmask);
    expectWrite(addrGateway, gateway);
    expectWrite(addrMac, mac);

    setupDevice(*device, config);
}
