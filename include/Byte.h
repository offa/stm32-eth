/*
 * Stm32 Eth - Ethernet connectivity for Stm32
 * Copyright (C) 2016-2018  offa
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

#include <cstdint>
#include <type_traits>
#include <iterator>

namespace eth::byte
{

    template<class T>
    constexpr bool inline is_byte_compatible_v = std::is_convertible_v<std::remove_cv_t<T>, std::uint8_t>
                                        && std::is_integral_v<T>;

    template<class Itr>
    constexpr bool inline is_byte_compatible_itr_v = is_byte_compatible_v<typename std::iterator_traits<Itr>::value_type>;


    template<std::size_t pos, class T,
        std::enable_if_t<std::is_integral<T>::value
                            && ( pos < sizeof(T) ), int> = 0>
    constexpr std::uint8_t get(T value) noexcept
    {
        constexpr auto shift{pos * 8};
        constexpr auto mask{0xff << shift};
        return ( value & mask ) >> shift;
    }


    template<class T, class U,
        std::enable_if_t<!is_byte_compatible_v<U>, int> = 0>
    constexpr void to(U) noexcept
    {
        static_assert(is_byte_compatible_v<U>, "Invalid type for 'U'");
    }

    template<class T, class U,
        std::enable_if_t<is_byte_compatible_v<U>, int> = 0,
        std::enable_if_t<std::is_integral_v<T>
                            && ( sizeof(T) >= sizeof(std::uint8_t) ), int> = 0>
    constexpr T to(U value) noexcept
    {
        return value;
    }

    template<class T, class U, class... Us,
        std::enable_if_t<is_byte_compatible_v<U>, int> = 0,
        std::enable_if_t<std::is_integral_v<T>
                            && ( sizeof(T) >= (sizeof...(Us) + sizeof(std::uint8_t)) ), int> = 0>
    constexpr T to(U valueN, Us... values) noexcept
    {
        constexpr auto shift{sizeof...(values) * 8};
        return ( valueN << shift ) | to<T>(values...);
    }

}

