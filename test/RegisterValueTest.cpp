/*
 * Stm32 Eth - Ethernet connectivity for Stm32
 * Copyright (C) 2016-2025  offa
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


#include "Protocol.h"
#include "Mode.h"
#include "SocketCommand.h"
#include "SocketInterrupt.h"
#include "SocketStatus.h"
#include <type_traits>
#include <CppUTest/TestHarness.h>

TEST_GROUP(RegisterValueTest)
{
    template <class E>
    constexpr auto value(E e)
    {
        return static_cast<std::underlying_type_t<E>>(e);
    }
};

TEST(RegisterValueTest, protocol)
{
    using eth::Protocol;
    CHECK_EQUAL(0x01, value(Protocol::tcp));
    CHECK_EQUAL(0x02, value(Protocol::udp));
    CHECK_EQUAL(0x03, value(Protocol::ipRaw));
    CHECK_EQUAL(0x04, value(Protocol::macRaw));
    CHECK_EQUAL(0x05, value(Protocol::pppoe));
}

TEST(RegisterValueTest, mode)
{
    using eth::Mode;
    CHECK_EQUAL(0x01, value(Mode::indirect));
    CHECK_EQUAL(0x02, value(Mode::autoIncrement));
    CHECK_EQUAL(0x08, value(Mode::pppoe));
    CHECK_EQUAL(0x10, value(Mode::pingBlock));
    CHECK_EQUAL(0x80, value(Mode::reset));
}

TEST(RegisterValueTest, socketCommand)
{
    using eth::SocketCommand;
    CHECK_EQUAL(0x00, value(SocketCommand::executed));
    CHECK_EQUAL(0x01, value(SocketCommand::open));
    CHECK_EQUAL(0x02, value(SocketCommand::listen));
    CHECK_EQUAL(0x04, value(SocketCommand::connect));
    CHECK_EQUAL(0x08, value(SocketCommand::disconnect));
    CHECK_EQUAL(0x10, value(SocketCommand::close));
    CHECK_EQUAL(0x20, value(SocketCommand::send));
    CHECK_EQUAL(0x21, value(SocketCommand::sendMac));
    CHECK_EQUAL(0x22, value(SocketCommand::sendKeep));
    CHECK_EQUAL(0x40, value(SocketCommand::receive));
}

TEST(RegisterValueTest, socketInterrupt)
{
    using eth::SocketInterrupt;
    CHECK_EQUAL(0x01, value(SocketInterrupt::Mask::connect));
    CHECK_EQUAL(0x02, value(SocketInterrupt::Mask::disconnect));
    CHECK_EQUAL(0x04, value(SocketInterrupt::Mask::receive));
    CHECK_EQUAL(0x08, value(SocketInterrupt::Mask::timeout));
    CHECK_EQUAL(0x10, value(SocketInterrupt::Mask::send));
}

TEST(RegisterValueTest, socketStatus)
{
    using eth::SocketStatus;
    CHECK_EQUAL(0x00, value(SocketStatus::closed));
    CHECK_EQUAL(0x13, value(SocketStatus::init));
    CHECK_EQUAL(0x14, value(SocketStatus::listen));
    CHECK_EQUAL(0x15, value(SocketStatus::synSent));
    CHECK_EQUAL(0x16, value(SocketStatus::synRecv));
    CHECK_EQUAL(0x17, value(SocketStatus::established));
    CHECK_EQUAL(0x18, value(SocketStatus::finWait));
    CHECK_EQUAL(0x1a, value(SocketStatus::closing));
    CHECK_EQUAL(0x1b, value(SocketStatus::timeWait));
    CHECK_EQUAL(0x1c, value(SocketStatus::closeWait));
    CHECK_EQUAL(0x1d, value(SocketStatus::lastAck));
    CHECK_EQUAL(0x22, value(SocketStatus::udp));
    CHECK_EQUAL(0x32, value(SocketStatus::ipRaw));
    CHECK_EQUAL(0x42, value(SocketStatus::macRaw));
    CHECK_EQUAL(0x5f, value(SocketStatus::pppoe));
}
