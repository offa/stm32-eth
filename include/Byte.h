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

        template<class T>
        constexpr bool is_byte_compatible_v = std::is_convertible<std::remove_cv_t<T>, uint8_t>::value
                                            && std::is_integral<T>::value;


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
            std::enable_if_t<!is_byte_compatible_v<U>, int> = 0>
        constexpr void to(U)
        {
            static_assert(is_byte_compatible_v<U>, "Invalid type for 'U'");
        }

        template<class T, class U,
            std::enable_if_t<is_byte_compatible_v<U>, int> = 0,
            std::enable_if_t<std::is_integral<T>::value
                                && ( sizeof(T) >= sizeof(uint8_t) ), int> = 0>
        constexpr T to(U value)
        {
            return value;
        }


        template<class T, class U, class... Us,
            std::enable_if_t<is_byte_compatible_v<U>, int> = 0,
            std::enable_if_t<std::is_integral<T>::value
                                && ( sizeof(T) >= (sizeof...(Us) + sizeof(uint8_t)) ), int> = 0>
        constexpr T to(U valueN, Us... values)
        {
            constexpr auto shift = sizeof...(values) * 8;
            const auto lower = to<T>(values...);
            return ( valueN << shift ) | lower ;
        }

    }
}

