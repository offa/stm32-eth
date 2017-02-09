# [Stm32 Eth](https://github.com/offa/stm32-eth)

[![Build Status](https://travis-ci.org/offa/stm32-eth.svg?branch=master)](https://travis-ci.org/offa/stm32-eth)
[![GitHub release](https://img.shields.io/github/release/offa/stm32-eth.svg)](https://github.com/offa/stm32-eth/releases)
[![License](https://img.shields.io/badge/license-GPLv3-yellow.svg)](LICENSE)
![C++](https://img.shields.io/badge/c++-17-green.svg)

Ethernet connectivity for *Stm32F4* Boards using W5100 Ethernet controller.

---------------------------
> ### Note
>
> This is the ***experimental* C++17** branch, which requires a capable compiler.

---------------------------

## Requirements

 - [**CMake**](http://www.cmake.org/)
 - [**GSL**](https://github.com/microsoft/gsl)
 - [**CppUTest**](https://github.com/cpputest/cpputest) – *(Optional)*
 - [**GNU ARM Embedded Toolchain**](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm) (*arm-none-eabi-gcc*)


## Testing

Build and execute Unit Tests:

```
mkdir build && cd build
cmake ..
make
make unittest
```

Executing tests using CTest: `make test` or `ctest`. For `unittest` it's possible to switch verbosity by CMake option.


## Integration Test

Integration Test for *Stm32F4* are enabled by the `INTEGRATION_TEST` option. The target `eth-stm32-it` is available as *ELF* (default) and *HEX*.


## Flashing (OpenOCD)

Both *ELF*- and *HEX*-files can be flashed using [***OpenOCD***](http://openocd.org/):

1. `openocd -f stm32f4discovery.cfg`
1. `telnet localhost 4444`
 1. `reset halt`
 1. `flash write_image erase <filename.[elf|hex]>`
 1. `reset run`


## Pin assignment

#### W5100

Connect ***Reset*** and ***3.3 V*** with **4.7 kΩ Resistor**.

[![W5100 Pin Assignment](doc/W5100-Pin-Assignment-Scaled.png?raw=true)](doc/W5100-Pin-Assignment.png?raw=true)

#### Stm32

| Pin  | Stm32 |
|:----:|:-----:|
| MOSI | PB15  |
| MISO | PB14  |
| SCK  | PB13  |
| SS   | PB12  |




## License

**GNU General Public License (GPL)**

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.


### Third party licenses

The License does ***not*** apply to files of `system` directory. Files included in the `system` folder are licensed under different conditions – please refer to it's files for more details.

