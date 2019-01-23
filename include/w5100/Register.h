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

#pragma once

#include "SocketHandle.h"
#include <cstdint>

namespace eth::w5100
{

    template<class T>
    class Register
    {
    public:

        using value_type = T;


        constexpr explicit Register(std::uint16_t address) noexcept : regAddress(address)
        {
        }


        constexpr std::uint16_t address() const noexcept
        {
            return regAddress;
        }



    private:

        const std::uint16_t regAddress;
    };


    template<class T>
    constexpr auto makeRegister(std::uint16_t address) noexcept
    {
        return Register<T>{address};
    }

    template<class T>
    constexpr auto makeRegister(SocketHandle s, std::uint16_t address) noexcept
    {
        constexpr std::uint16_t baseAddress = 0x0400;
        constexpr std::uint16_t channelRegisterMapSize = 0x0100;
        return makeRegister<T>(baseAddress + ( s.value() * channelRegisterMapSize ) + address);
    }

}
