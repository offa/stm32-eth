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

#include "Byte.h"
#include <CppUTest/TestHarness.h>

using namespace eth;

TEST_GROUP(ByteTest)
{
    void setup() override
    {
    }

    void teardown() override
    {
    }
};

TEST(ByteTest, getByteUint8)
{
    constexpr uint8_t value = 0xab;
    CHECK_EQUAL(0xab, byte::get<0>(value));
}

TEST(ByteTest, getByteUint16)
{
    constexpr uint16_t value = 0xabcd;
    CHECK_EQUAL(0xcd, byte::get<0>(value));
    CHECK_EQUAL(0xab, byte::get<1>(value));
}

TEST(ByteTest, getByteUint32)
{
    constexpr uint32_t value = 0xabcdef01;
    CHECK_EQUAL(0x01, byte::get<0>(value));
    CHECK_EQUAL(0xef, byte::get<1>(value));
    CHECK_EQUAL(0xcd, byte::get<2>(value));
    CHECK_EQUAL(0xab, byte::get<3>(value));
}

TEST(ByteTest, getByteWithLessValue)
{
    constexpr uint16_t value = 0xab;
    CHECK_EQUAL(0xab, byte::get<0>(value));
    CHECK_EQUAL(0x00, byte::get<1>(value));
}

TEST(ByteTest, toByteUint8)
{
    constexpr uint8_t b0 = 0xab;
    CHECK_EQUAL(0xab, byte::to<uint8_t>(b0));
}

TEST(ByteTest, toByteUint16)
{
    constexpr uint8_t b0 = 0xcd;
    CHECK_EQUAL(0xabcd, byte::to<uint16_t>(0xab, b0));
}

TEST(ByteTest, toByteUint32)
{
    constexpr uint8_t b0 = 0x01;
    constexpr uint8_t b3 = 0xab;
    CHECK_EQUAL(0xabcdef01, byte::to<uint32_t>(b3, 0xcd, 0xef, b0));
}

TEST(ByteTest, toByteWithLessThanSize)
{
    constexpr uint8_t b0 = 0x01;
    constexpr uint8_t b1 = 0x02;
    CHECK_EQUAL(0x0201, byte::to<uint32_t>(b1, b0));
}

