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

#ifndef W5100DEVICE_H
#define W5100DEVICE_H

#include <stdint.h>
#include "Spi.h"
#include <SocketType.h>

namespace eth
{

    class W5100Device
    {
    public:

        W5100Device();

        void init();

        void executeSocketCommand(Socket s, uint8_t value);

        void writeSocketModeRegister(Socket s, uint8_t value);
        void writeSocketSourcePort(Socket s, uint16_t value);

        void writeSocketInterruptRegister(Socket s, uint8_t value);
        uint8_t readSocketInterruptRegister(Socket s);

        void writeSocketCommandRegister(Socket s, uint8_t value);
        uint8_t readSocketCommandRegister(Socket s);

        uint8_t readSocketStatusRegister(Socket s);

        uint16_t getTransmitFreeSize(Socket s);

        void sendData(Socket s, const uint8_t* buffer, uint16_t size);


        void write(uint16_t addr, uint8_t data);
        void write(uint16_t addr, const uint8_t* buffer, uint16_t size);
        uint8_t read(uint16_t addr);

        void writeModeRegister(uint8_t value);

        void setGatewayAddress(uint8_t* addr);
        void setSubnetMask(uint8_t* addr);
        void setMacAddress(uint8_t* addr);
        void setIpAddress(uint8_t* addr);


        static constexpr uint16_t transmitBufferSize = 2048;


    protected:

        uint16_t readSocketTransmitFreeSizeRegister(Socket s);

        uint16_t readSocketTransmitWritePointer(Socket s);
        void writeSocketTransmitWritePointer(Socket s, uint16_t value);

        void writeTransmitMemorySizeRegister(uint8_t value);
        void writeReceiveMemorySizeRegister(uint8_t value);

        void writeGatewayAddressRegister(uint8_t* addr);
        void writeSubnetMaskRegister(uint8_t* addr);
        void writeSourceMacAddressRegister(uint8_t* addr);
        void writeSourceIpRegister(uint8_t* addr);


    private:

        uint16_t m_transmitBufferBaseAddress[supportedSockets];

        static constexpr uint16_t socketRegisterBaseAddress = 0x0400;
        static constexpr uint16_t socketChannelRegisterMapSize = 0x0100;
        static constexpr uint16_t transmitBufferMask = 0x07ff;
    };


    extern W5100Device device;

}

#endif /* W5100DEVICE_H */
