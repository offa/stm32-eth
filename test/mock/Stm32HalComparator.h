/*
 * Stm32 Eth - Ethernet connectivity for Stm32
 * Copyright (C) 2016-2024  offa
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

#include "Platform.h"
#include <sstream>
#include <CppUTestExt/MockSupport.h>

struct GpioInitComparator : public MockNamedValueComparator
{
    bool isEqual(const void* object1, const void* object2) override
    {
        const auto* value1 = static_cast<const GPIO_InitTypeDef*>(object1);
        const auto* value2 = static_cast<const GPIO_InitTypeDef*>(object2);

        return (std::tie(value1->Pin, value1->Mode, value1->Pull, value1->Speed, value1->Alternate) ==
                std::tie(value2->Pin, value2->Mode, value2->Pull, value2->Speed, value2->Alternate));
    }

    SimpleString valueToString(const void* object) override
    {
        const auto* value = static_cast<const GPIO_InitTypeDef*>(object);

        std::stringstream ss;
        ss << "SPI_InitTypeDef { "
           << "Pin: " << value->Pin << ", Mode: " << value->Mode << ", Pull: " << value->Pull << ", Speed: " << value->Speed
           << ", Alternate: " << value->Alternate << " }";

        return StringFrom(ss.str());
    }
};


struct SpiHandleComparator : public MockNamedValueComparator
{
    bool isEqual(const void* object1, const void* object2) override
    {
        const auto* value1 = static_cast<const SPI_InitTypeDef*>(object1);
        const auto* value2 = static_cast<const SPI_InitTypeDef*>(object2);

        return (std::tie(value1->Mode, value1->Direction, value1->DataSize, value1->CLKPolarity, value1->CLKPhase, value1->NSS,
                         value1->BaudRatePrescaler, value1->FirstBit, value1->TIMode, value1->CRCCalculation,
                         value1->CRCPolynomial) == std::tie(value2->Mode, value2->Direction, value2->DataSize,
                                                            value2->CLKPolarity, value2->CLKPhase, value2->NSS,
                                                            value2->BaudRatePrescaler, value2->FirstBit, value2->TIMode,
                                                            value2->CRCCalculation, value2->CRCPolynomial));
    }

    SimpleString valueToString(const void* object) override
    {
        const auto* value = static_cast<const SPI_InitTypeDef*>(object);

        std::stringstream ss;
        ss << "SPI_InitTypeDef { "
           << "Mode: " << value->Mode << ", Direction: " << value->Direction << ", DataSize: " << value->DataSize
           << ", CLKPolarity: " << value->CLKPolarity << ", CLKPhase: " << value->CLKPhase << ", NSS: " << value->NSS
           << ", BaudRatePrescaler: " << value->BaudRatePrescaler << ", FirstBit: " << value->FirstBit
           << ", TIMode: " << value->TIMode << ", CRCCalculation: " << value->CRCCalculation
           << ", CRCPolynomial: " << value->CRCPolynomial << " }";

        return StringFrom(ss.str());
    }
};
