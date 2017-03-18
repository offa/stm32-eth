/*
 * Stm32 Eth - Ethernet connectivity for Stm32
 * Copyright (C) 2016 - 2017  offa
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

#include "spi/SpiWriter.h"
#include <CppUTestExt/MockSupport.h>

namespace eth
{
namespace spi
{

    namespace mockutil
    {
        static inline void incrementCalls(const ::SimpleString& name)
        {
            const int count = mock("SpiWriter").getData(name).getUnsignedIntValue();
            mock("SpiWriter").setData(name, count + 1);
        }
    }


    SpiWriter::SpiWriter(const SpiConfig&)
    {
    }

    void SpiWriter::write(uint16_t address, uint8_t data)
    {
        mockutil::incrementCalls("write::count");

        mock("SpiWriter").actualCall("write")
                .withParameter("address", address)
                .withParameter("data", data);
    }

    uint8_t SpiWriter::read(uint16_t address)
    {
        mockutil::incrementCalls("read::count");

        return mock("SpiWriter").actualCall("read")
                .withParameter("address", address)
                .returnUnsignedIntValue();
    }

}
}
