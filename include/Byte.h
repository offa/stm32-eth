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

#include <stddef.h>
#include <stdint.h>
#include <type_traits>

namespace eth
{
    namespace byte
    {

        template<size_t pos, class T,
            std::enable_if_t<std::is_integral<T>::value
                                && ( pos < sizeof(T) ), int> = 0>
        constexpr uint8_t get(T value)
        {
            constexpr auto shift = pos * 8;
            constexpr auto mask = ( 0xff << shift );
            return ( value & mask ) >> shift;
        }


        template<class T, class U,
            std::enable_if_t<std::is_integral<T>::value
                                && ( sizeof(T) >= sizeof(uint8_t) ), int> = 0,
            std::enable_if_t<std::is_integral<U>::value
                                && ( sizeof(U) != sizeof(uint8_t) ), int> = 0>
        constexpr void to(U value)
        {
            static_assert(sizeof(value) == sizeof(uint8_t), "Invalid size");
        }

        template<class T,
            std::enable_if_t<std::is_integral<T>::value
                                && ( sizeof(T) >= sizeof(uint8_t) ), int> = 0>
        constexpr T to(uint8_t value)
        {
            return value;
        }

        template<class T, class... Ts,
            std::enable_if_t<std::is_integral<T>::value
                                && ( sizeof(T) >= (sizeof...(Ts) + sizeof(uint8_t)) ), int> = 0>
        constexpr T to(uint8_t valueN, Ts... values)
        {
            constexpr auto shift = sizeof...(values) * 8;
            auto lower = to<T>(values...);
            return ( valueN << shift ) | lower ;
        }

    }
}

