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

#include "SocketHandle.h"
#include "SocketCommand.h"
#include "SocketStatus.h"
#include <array>
#include <stdint.h>


namespace eth
{

    class W5100Device
    {
    public:

        W5100Device();
        W5100Device(W5100Device&&) = default;

        void init();

        void executeSocketCommand(SocketHandle s, SocketCommand cmd);

        void writeSocketModeRegister(SocketHandle s, uint8_t value);
        void writeSocketSourcePort(SocketHandle s, uint16_t value);

        void writeSocketInterruptRegister(SocketHandle s, uint8_t value);
        uint8_t readSocketInterruptRegister(SocketHandle s);

        void writeSocketCommandRegister(SocketHandle s, SocketCommand value);
        SocketCommand readSocketCommandRegister(SocketHandle s);

        SocketStatus readSocketStatusRegister(SocketHandle s);

        uint16_t getTransmitFreeSize(SocketHandle s);

        void sendData(SocketHandle s, const uint8_t* buffer, uint16_t size);


        void write(uint16_t addr, uint8_t data);
        void write(uint16_t addr, const uint8_t* buffer, uint16_t size);
        uint8_t read(uint16_t addr);

        void writeModeRegister(uint8_t value);

        void setGatewayAddress(const std::array<uint8_t, 4>& addr);
        void setSubnetMask(const std::array<uint8_t, 4>& addr);
        void setMacAddress(const std::array<uint8_t, 6>& addr);
        void setIpAddress(const std::array<uint8_t, 4>& addr);

        static constexpr uint16_t getTransmitBufferSize()
        {
            return transmitBufferSize;
        }

        W5100Device& operator=(W5100Device&&) = default;


    protected:

        uint16_t readSocketTransmitFreeSizeRegister(SocketHandle s);

        uint16_t readSocketTransmitWritePointer(SocketHandle s);
        void writeSocketTransmitWritePointer(SocketHandle s, uint16_t value);

        void writeTransmitMemorySizeRegister(uint8_t value);
        void writeReceiveMemorySizeRegister(uint8_t value);

        void writeGatewayAddressRegister(const std::array<uint8_t, 4>& addr);
        void writeSubnetMaskRegister(const std::array<uint8_t, 4>& addr);
        void writeSourceMacAddressRegister(const std::array<uint8_t, 6>& addr);
        void writeSourceIpRegister(const std::array<uint8_t, 4>& addr);


    private:

        std::array<uint16_t, supportedSockets> m_transmitBufferBaseAddress;

        static constexpr uint8_t opcodeWrite = 0xf0;
        static constexpr uint8_t opcodeRead = 0x0f;
        static constexpr uint16_t transmitBufferSize = 2048;
    };


    extern W5100Device device;

}

#endif /* W5100DEVICE_H */
