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

#pragma once

#include "SocketHandle.h"
#include <stdint.h>

namespace eth
{
    namespace w5100
    {

        template<class T>
        class Register
        {
        public:

            using value_type = T;


            constexpr Register(uint16_t address) : m_address(address)
            {
            }


            constexpr uint16_t address() const
            {
                return m_address;
            }



        private:

            const uint16_t m_address;
        };


        template<class T>
        constexpr auto makeRegister(uint16_t address)
        {
            return Register<T>(address);
        }

        template<class T>
        constexpr auto makeRegister(SocketHandle s, uint16_t address)
        {
            constexpr uint16_t baseAddress = 0x0400;
            constexpr uint16_t channelRegisterMapSize = 0x0100;
            return makeRegister<T>(baseAddress + ( s.value() * channelRegisterMapSize ) + address);
        }

    }
}
