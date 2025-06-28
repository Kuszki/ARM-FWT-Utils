# ARM-FWT-Utils

A few implementations of `DWT` algorithm using `CMSIS-DSP` library for `ARM`.

## Purpose

The main purpose of this project is to find the best implementation of `DWT` algorithm in case of using `ARM` with `CMSIS-DSP`. There are many possible solutions to implement `DWT`, where some are fast while the others are simple. All of `DWT` implementations contained in this project are able to be used in `ARM` meanwile they can be tested on any `GCC` compatibile platform.

## Build

To build entire project and run all tests you need to:

``` bash
git clone https://github.com/Kuszki/ARM-FWT-Utils # clone this repo
cd ARM-FWT-Utils && ./build.sh                    # run build script
```

The default build script configures the project using `CMake`, generates all data required to run tests and performs all tests. 

You can still use `CMake` by yourself as the `build.sh` only invokes `CMake` to use `Ninja` and `build-dir` as build directory and runs `ctest` after every build.

## Credits

- [Octave-FWT-Utils](https://github.com/Kuszki/Octave-FWT-Utils) -- GPL-3 -- some helpers for `GNU Octave`.
- [CMSIS-DSP](https://github.com/ARM-software/CMSIS-DSP) -- Apache 2.0 -- implementation of `CMSIS-DSP`.

## License

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. 

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. 

You should have received a copy of the GNU General Public License along with this program. If not, see http://www.gnu.org/licenses/. 
