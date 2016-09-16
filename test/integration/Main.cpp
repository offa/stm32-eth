
#include <stm32f4xx_hal.h>
#include <diag/Trace.h>
#include "Socket.h"
#include "W5100Device.h"
#include "Spi.h"


namespace eth
{
    Spi spi;
}



int main(int argc, char* argv[])
{
    (void) argc;
    (void) argv;

    HAL_Init();

    eth::spi.init();

    eth::device.init();
    eth::device.setGatewayAddress({192, 168, 1, 1});
    eth::device.setIpAddress({192, 168, 1, 8});
    eth::device.setSubnetMask({255, 255, 255, 0});
    eth::device.setMacAddress({0x00, 0x08, 0xdc, 0xab, 0xcd, 0xef});

    constexpr eth::SocketHandle handle = 0;
    eth::Socket socket(handle);
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


        while( eth::device.readSocketStatusRegister(handle) == eth::SocketStatus::listen )
        {
            HAL_Delay(100);
        }

        trace_puts("listen() done");


        while(true)
        {
            if( eth::device.readSocketStatusRegister(handle) == eth::SocketStatus::established )
            {
                const std::string msg = "*** Test *** Test *** Test ***\n\r";
                auto n = socket.send(reinterpret_cast<const uint8_t*>(msg.c_str()), msg.size());

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


