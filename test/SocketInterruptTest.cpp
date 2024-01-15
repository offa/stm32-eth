/*
 * Stm32 Eth - Ethernet connectivity for Stm32
 * Copyright (C) 2016-2024  offa
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

#include "SocketInterrupt.h"
#include <CppUTest/TestHarness.h>

using eth::SocketInterrupt;

TEST_GROUP(SocketInterruptTest)
{
};

TEST(SocketInterruptTest, defaultNothingSet)
{
    SocketInterrupt si{};
    CHECK_EQUAL(0xff, si.value());
}

TEST(SocketInterruptTest, initWithValue)
{
    SocketInterrupt si{0xab};
    CHECK_EQUAL(0xab, si.value());
}

TEST(SocketInterruptTest, initWithMask)
{
    constexpr auto mask = SocketInterrupt::Mask::send;
    SocketInterrupt si{mask};
    CHECK_EQUAL(static_cast<std::uint8_t>(mask), si.value());
}

TEST(SocketInterruptTest, testValue)
{
    using Mask = SocketInterrupt::Mask;

    SocketInterrupt si{0b00011001};
    CHECK_TRUE(si.test(Mask::connect));
    CHECK_FALSE(si.test(Mask::disconnect));
    CHECK_FALSE(si.test(Mask::receive));
    CHECK_TRUE(si.test(Mask::timeout));
    CHECK_TRUE(si.test(Mask::send));
}

TEST(SocketInterruptTest, setValue)
{
    using Mask = SocketInterrupt::Mask;

    SocketInterrupt si{0x00};
    si.set(Mask::disconnect);
    si.set(Mask::receive);
    si.set(Mask::timeout);
    CHECK_EQUAL(0b00001110, si.value());
}
