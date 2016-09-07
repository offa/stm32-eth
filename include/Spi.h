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

#ifndef SPI_H
#define SPI_H

#include <stdint.h>
#include "Platform.h"

namespace eth
{

    class Spi
    {
    public:

        using Handle = SPI_HandleTypeDef;


        void init();

        void transmit(uint8_t data);
        uint8_t receive();

        void setSlaveSelect();
        void resetSlaveSelect();

        Handle& nativeHandle();


   private:

        Handle m_handle{};
        static constexpr uint32_t timeout = ~0;
    };

    extern Spi spi;

}

#endif /* SPI_H */

