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

#include "Spi.h"
#include <CppUTestExt/MockSupport.h>

namespace eth
{
    namespace mockutil
    {
        static inline void incrementCalls(const ::SimpleString& name)
        {
            int count = mock("Spi").getData(name).getUnsignedIntValue();
            ++count;
            mock("Spi").setData(name, count);
        }
    }
    

    uint8_t Spi::transfer(uint8_t data)
    {
        mockutil::incrementCalls("transfer::count");
        
        return mock("Spi").actualCall("transfer")
                .withParameter("data", data)
                .returnUnsignedIntValueOrDefault(0xff);
    }

    void Spi::setSlaveSelect()
    {
        mock("Spi").actualCall("setSlaveSelect");
    }

    void Spi::resetSlaveSelect()
    {
        mock("Spi").actualCall("resetSlaveSelect");
    }


    Spi spi;

}