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

#include <stm32f4xx_hal.h>
#include <diag/Trace.h>
#include "Socket.h"
#include "W5100Device.h"
#include "SpiWriter.h"


int main(int argc, char* argv[])
{
    (void) argc;
    (void) argv;

    HAL_Init();

    eth::SpiWriter writer;

    eth::W5100Device device(writer);
    device.init();
    device.setGatewayAddress({192, 168, 1, 1});
    device.setIpAddress({192, 168, 1, 8});
    device.setSubnetMask({255, 255, 255, 0});
    device.setMacAddress({0x00, 0x08, 0xdc, 0xab, 0xcd, 0xef});

    constexpr eth::SocketHandle handle = 0;
    eth::Socket socket(handle, device);
    constexpr uint16_t port = 5000;

    while(true)
    {
        if( socket.open(eth::Protocol::tcp, port, 0) == false )
        {
            trace_puts("socket() failed");
        }
        else
        {
            trace_puts("socket() ok");
        }


        if( socket.listen() == false )
        {
            trace_puts("listen() failed");
        }
        else
        {
            trace_puts("listen() ok");
        }


        socket.accept();
        trace_puts("accept() done");


        while(true)
        {
            if( socket.getStatus() == eth::SocketStatus::established )
            {
                std::array<uint8_t, 6> msg{'a', 'b', 'c', 'd', '\n', '\r'};
                auto n = socket.send(msg);

                if( n != msg.size() )
                {
                    trace_puts("send() failed");
                }
                else
                {
                    trace_puts("send() ok");
                }
            }

        }

    }

    return 0;
}


extern "C" void SysTick_Handler(void)
{
    HAL_IncTick();
}


