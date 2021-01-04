/*
 * Stm32 Eth - Ethernet connectivity for Stm32
 * Copyright (C) 2016-2021  offa
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

#include "Byte.h"
#include <CppUTest/TestHarness.h>

TEST_GROUP(ByteTest)
{
};

TEST(ByteTest, getByteUint8)
{
    constexpr std::uint8_t value{0xab};
    CHECK_EQUAL(0xab, eth::byte::get<0>(value));
}

TEST(ByteTest, getByteUint16)
{
    constexpr std::uint16_t value{0xabcd};
    CHECK_EQUAL(0xcd, eth::byte::get<0>(value));
    CHECK_EQUAL(0xab, eth::byte::get<1>(value));
}

TEST(ByteTest, getByteUint32)
{
    constexpr std::uint32_t value{0xabcdef01};
    CHECK_EQUAL(0x01, eth::byte::get<0>(value));
    CHECK_EQUAL(0xef, eth::byte::get<1>(value));
    CHECK_EQUAL(0xcd, eth::byte::get<2>(value));
    CHECK_EQUAL(0xab, eth::byte::get<3>(value));
}

TEST(ByteTest, getByteWithLessValue)
{
    constexpr std::uint16_t value{0xab};
    CHECK_EQUAL(0xab, eth::byte::get<0>(value));
    CHECK_EQUAL(0x00, eth::byte::get<1>(value));
}

TEST(ByteTest, toByteUint8)
{
    constexpr std::uint8_t b0{0xab};
    CHECK_EQUAL(0xab, eth::byte::to<std::uint8_t>(b0));
}

TEST(ByteTest, toByteUint16)
{
    constexpr std::uint8_t b0{0xcd};
    CHECK_EQUAL(0xabcd, eth::byte::to<std::uint16_t>(0xab, b0));
}

TEST(ByteTest, toByteUint32)
{
    constexpr std::uint8_t b0{0x01};
    constexpr std::uint8_t b3{0xab};
    CHECK_EQUAL(0xabcdef01, eth::byte::to<std::uint32_t>(b3, 0xcd, 0xef, b0));
}

TEST(ByteTest, toByteWithLessThanSize)
{
    constexpr std::uint8_t b0{0x01};
    constexpr std::uint8_t b1{0x02};
    CHECK_EQUAL(0x0201, eth::byte::to<std::uint32_t>(b1, b0));
}
