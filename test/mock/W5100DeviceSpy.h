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

#ifndef W5100DEVICESPY_H
#define W5100DEVICESPY_H

#include "W5100Device.h"

namespace test
{

    class W5100DeviceSpy : public eth::W5100Device
    {
        public:

            uint16_t spy_readSocketTransmitFreeSizeRegister(eth::Socket s)
            {
                return readSocketTransmitFreeSizeRegister(s);
            }

            void spy_writeGatewayAddressRegister(uint8_t* addr)
            {
                writeGatewayAddressRegister(addr);
            }

            void spy_writeSubnetMaskRegister(uint8_t* addr)
            {
                writeSubnetMaskRegister(addr);
            }

            void spy_writeSourceMacAddressRegister(uint8_t* addr)
            {
                writeSourceMacAddressRegister(addr);
            }

            void spy_writeSourceIpRegister(uint8_t* addr)
            {
                writeSourceIpRegister(addr);
            }

            uint16_t spy_readSocketTransmitWritePointer(eth::Socket s)
            {
                return readSocketTransmitWritePointer(s);
            }

            void spy_writeSocketTransmitWritePointer(eth::Socket s, uint16_t value)
            {
                writeSocketTransmitWritePointer(s, value);
            }

            void spy_writeTransmitMemorySizeRegister(uint8_t value)
            {
                writeTransmitMemorySizeRegister(value);
            }

            void spy_writeReceiveMemorySizeRegister(uint8_t value)
            {
                writeReceiveMemorySizeRegister(value);
            }

    };

}

#endif /* W5100DEVICESPY_H */

