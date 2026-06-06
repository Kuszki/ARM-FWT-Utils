# ARM-FWT-Utils

Standalone implementation of `FWT` algorithm compatible with `CMSIS-DSP` library for `Cortex-M4` chips.

## Purpose

The main purpose of this project is to find the best implementation of `FWT` algorithm in case of using `ARM` without direct `CMSIS-DSP` usage. There are many possible solutions to implement `DWT`, where some are fast while the others are simple. All of `DWT` implementations contained in this project are able to be used in `ARM Cortex-M4` meanwhile they can be tested on any `GCC` compatible platform. Neither this project doesn't use `CMSIS-DSP` library it's written based on `CMSIS-DSP` style and optimizations.

This project does not depend on any external library directly and uses only `malloc` from `stdlib` to prepare algorithm parameter's depended resources. All requirements are detailed in further section.

## Documentation

- Floating point version -- `arm_wt_q15_class`
- Fixed point version -- `arm_wt_f32_class`

To generate documentation use `Doxygen` with provided `Doxyfile`.

## Project structure

There are several directories containing:
- `inc-dir` -- all header files
- `src-dir` -- all source files
- `test-dir` -- all `ctest` source files
- `octave-dir` -- all octave files implementing the same algorithms to human-invoked tests and scripts to generate data for tests

The main `WT` algorithm class is implemented in files named `arm_wt_*`. In case of usage in any system you only need to copy:
- `arm_wt_common.h` -- in all cases to provide common definitions
- `arm_wt_f32.c` and `arm_wt_f32.h` -- for floating point version
- `arm_wt_f32.c` and `arm_wt_f32.c`-- for fixed point version

All other files are required to perform test in desktop environment and are not required in case of embedded system usage.

You can find `Doxygen`-like comments for all `arm_wt` related objects and `Doxyfile` to prepare documentation.

## Build

To build entire project on any desktop platform and run all tests you need to:

``` bash
git clone https://github.com/Kuszki/ARM-FWT-Utils # clone this repo
cd ARM-FWT-Utils && ./build.sh                    # run build script
```

The default build script configures the project using `CMake`, generates all data required to run tests and performs all tests. 

You can still use `CMake` by yourself as the `build.sh` only invokes `CMake` to use `Ninja` and `build-dir` as build directory and runs `ctest` after every build.

This project is not provided as prebuild library due large compiler based optimization options. Use `-Ofast` and optional `-flto` to keep the best performance.

## Requirements

Neither this library don't depend on any other project directly, it requires some base definitions. First one is platform independent explicit definitions of common data types, provided by `arm_math_types.h` header to keep the best compatibility with `CMSIS-DSP`. Second one is `arm_math_memory.h` to provide memory read/write functions to handle `SIMD` memory operations. The first two headers are linked with `CMSIS-DSP`. The last part is `cmsis_compiler.h`, required by `CMSIS-DSP` and to provide `SIMD` instructions wrappers, provided by `CMSIS-CORE`. As there is no `CMake` support in case of `CMSIS-CORE` all required headers are provided in `cmsis-core` directory.

## Credits

- [Octave-FWT-Utils](https://github.com/Kuszki/Octave-FWT-Utils) -- GPL-3 -- some helpers for `GNU Octave`.
- [CMSIS-CORE](https://github.com/ARM-software/CMSIS_6) -- Apache 2.0 -- implementation of `CMSIS-CORE`.
- [CMSIS-DSP](https://github.com/ARM-software/CMSIS-DSP) -- Apache 2.0 -- implementation of `CMSIS-DSP`.

## License

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. 

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. 

You should have received a copy of the GNU General Public License along with this program. If not, see http://www.gnu.org/licenses/. 

