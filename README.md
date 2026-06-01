# ARM-FWT-Utils

Standalone implementation of `FWT` algorithm compatible with `CMSIS-DSP` library.

## Purpose

The main purpose of this project is to find the best implementation of `FWT` algorithm in case of using `ARM` without `CMSIS-DSP`. There are many possible solutions to implement `DWT`, where some are fast while the others are simple. All of `DWT` implementations contained in this project are able to be used in `ARM` meanwhile they can be tested on any `GCC` compatible platform. Neither this project doesn't use `CMSIS-DSP` library it's written based on `CMSIS-DSP` style and optimizations.

This project does not depend on any external library and uses only `malloc` from `stdlib`. It only requires `arm_math_types.h` to provide explicit `float32_t`, `uint32_t` and `size_t` types.

## Project structure

There are several directories containing:
- `inc-dir` -- all header files
- `src-dir` -- all source files, including `GNU Octave` scripts required to run tests
- `test-dir` -- all `ctest` source files
- `octave-dir` -- all octave files implementing the same algorithms to human-invoked tests

The main `FWT` algorithm class is implemented in files named `arm_wt_f32` -- in case of usage in embedded system you only need `arm_wt_f32.c` and `arm_wt_f32.h` file.

All other files are required to perform test in desktop and are not required in case of embedded system usage.

You can find `Doxygen`-like comments for all `arm_wt_f32` related objects.

## Build

To build entire project on any desktop platform and run all tests you need to:

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

