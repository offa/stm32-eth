# Stm32 Eth

[![License](https://img.shields.io/badge/license-GPLv3-yellow.svg)](LICENSE)
[![C++](https://img.shields.io/badge/c++-14-green.svg)](http://www.open-std.org/jtc1/sc22/wg21/)

Ethernet connectivity for *Stm32F4xx* Boards using W5100 devices.


## Requirements

 - [**CMake**](http://www.cmake.org/)
 - [**CppUTest**](https://github.com/cpputest/cpputest) - *(Optional)*


## Testing

```
mkdir build && cd build
cmake ..
make
make unittest
```

It's also possible to use `make test` / `ctest` instead. For `unittest` it's possible to switch verbosity by CMake option.


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

