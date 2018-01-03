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

#include <stm32f4xx_hal.h>
#include <diag/Trace.h>
#include "Socket.h"
#include "w5100/Device.h"
#include "spi/SpiWriter.h"

void spiClockEnable()
{
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_SPI2_CLK_ENABLE();
}


int main(int argc, char* argv[])
{
    static_cast<void>(argc);
    static_cast<void>(argv);

    HAL_Init();
    spiClockEnable();

    constexpr auto config = eth::NetConfig{
        {{192, 168, 1, 1}},
        {{192, 168, 1, 8}},
        {{255, 255, 255, 0}},
        {{0x00, 0x08, 0xdc, 0xab, 0xcd, 0xef}}
    };

    eth::spi::SpiWriter writer(eth::spi::spi2);
    eth::w5100::Device device(writer);
    eth::w5100::setupDevice(device, config);

    eth::Socket socket(eth::makeHandle<0>(), device);
    constexpr std::uint16_t port = 5000;

    trace_puts("Server: 192.168.1.8:5000");

    while(true)
    {
        if( socket.open(eth::Protocol::tcp, port, 0) != eth::Socket::Status::ok )
        {
            trace_puts("socket() failed");
        }
        else
        {
            trace_puts("socket() ok");
        }


        if( socket.listen() != eth::Socket::Status::ok )
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
                std::array<std::uint8_t, 20> buffer;

                const auto received = socket.receive(buffer);
                trace_printf("receive(): %d\n", received);

                if( received > 0 )
                {
                    std::array<std::uint8_t, 9> resp{{ 'r', 'e', 'c', 'e', 'i', 'v', 'e', 'd', '\n' }};
                    const auto n = socket.send(resp);

                    if( n != resp.size() )
                    {
                        trace_printf("send() failed (%d)\n", n);
                    }
                    else
                    {
                        trace_printf("send() ok (%d / %d)\n", n, resp.size());
                    }
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


