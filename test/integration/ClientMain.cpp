/*
 * Stm32 Eth - Ethernet connectivity for Stm32
 * Copyright (C) 2016 - 2017  offa
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

    eth::spi::SpiWriter writer(eth::spi::spi2);

    eth::w5100::Device device(writer);
    device.setGatewayAddress({192, 168, 1, 1});
    device.setIpAddress({192, 168, 1, 8});
    device.setSubnetMask({255, 255, 255, 0});
    device.setMacAddress({0x00, 0x08, 0xdc, 0xab, 0xcd, 0xef});

    eth::Socket socket(eth::makeHandle<0>(), device);

    while(true)
    {
        if( socket.connect({192, 168, 1, 6}, 5000) != eth::Socket::Status::ok )
        {
            trace_puts("connect() failed");
        }
        else
        {
            trace_puts("connect() ok");
        }

        while(true)
        {
            if( socket.getStatus() == eth::SocketStatus::established )
            {
                const std::array<uint8_t, 6> msg{'a', 'b', 'c', 'd', '\n', '\r'};
                const auto n = socket.send(msg);

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


