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

#include "W5100Register.h"
#include "SocketHandle.h"
#include <array>
#include <gsl/span>
#include <CppUTest/TestHarness.h>

using namespace eth;

TEST_GROUP(W5100RegisterTest)
{
    void setup() override
    {
    }

    void teardown() override
    {
    }

};

TEST(W5100RegisterTest, address)
{
    constexpr W5100Register<uint16_t> reg(0xabcd);
    CHECK_EQUAL(0xabcd, reg.address());
}

TEST(W5100RegisterTest, makeRegister)
{
    constexpr auto reg = makeRegister<uint32_t>(0xaabb);
    CHECK_EQUAL(0xaabb, reg.address());
}

TEST(W5100RegisterTest, makeSocketRegister)
{
    constexpr uint16_t address = 0xaabb;
    constexpr uint16_t expected = 0x0400 + ( 1 * 0x0100 ) + address;
    constexpr auto reg = makeSocketRegister<uint32_t>(makeHandle<1>(), address);
    CHECK_EQUAL(expected, reg.address());
}

TEST(W5100RegisterTest, registerOfArray)
{
    const auto reg = makeRegister<std::array<uint8_t, 4>>(0x0011);
    CHECK_EQUAL(0x0011, reg.address());
}

TEST(W5100RegisterTest, registerOfSpan)
{
    const auto reg = makeRegister<gsl::span<uint8_t>>(0x1122);
    CHECK_EQUAL(0x1122, reg.address());
}

