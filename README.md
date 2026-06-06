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

## Usage

Before using you need to create proper instance of `arm_wt` object, depending on selected implementation. To do this fill required fields of data structure and call constructor:
``` c
// Define low pass FIR coefficients in reverse order
const float32_t coef_C[] = { +0.68301f, +1.18301f, +0.31699f, -0.18301f };

// Define high pass FIR coefficients in reverse order
const float32_t coef_B[] = { -0.18301f, -0.31699f, +1.18301f, -0.68301f };

// Number of coefficients -- len(C) must be equal to len(B)
const size_t NC = sizeof(coef_C) / sizeof(float32_t);

// Define buffer for ADC. Usually you need 2 * N buffer length to enable
// continuous computation. While ADC processes N samples the algorithm 
// compute previous N values. To make this example simple it isn't used.
const size_t N = 128; // Number of samples provided as input for single run
uint16_t ADC_out[N]; // Buffer where ADC stores raw signal samples

// Fill required fields of WT object before running constructor
arm_wt_f32_instance fwt = {
  .c = coef_C,    // hi pass coefficients
  .b = coef_B,    // lo pass coefficients
  .c_len = NC,    // number of coefficients
  .n_len = N,     // input data length for single run
  .n_dec = 2,     // number of decomposition iterations
  .scale = 1.0f,  // adc scale factor and bias value to
  .bias = 0.0f,   // compute in = adc * scale + bias
};

// Init all internal buffers and settings and check status
arm_wt_status s = arm_fwt_f32_init(&fwt);

if (s != WT_STATUS_SUCCESS) {} // error occurred
``` 

After initiation you can make computations and you can check the results:
``` c
arm_wt_f32_run(&fwt, ADC_out); // compute FWT and store outputs internally

for (size_t i = 0; i <= fwt.n_dec; ++i) // iterate by decomposition levels
{
  printf("\nlvl %zu (%zu samples):\n", i, fwt.lens[i]);
  
  for (size_t j = 0; j < fwt.lens[i]; ++j)
  {
  	printf("%+5.5f\t", fwt.out[i][j]);
  }
  
  printf("\n");
}
```

After everything is finished and the object instance is no longer needed you need to free all resources calling:
``` c
arm_wt_f32_free(&fwt);
```

If you don't need to remember previous filter state and you need only calculate FWT fast for provided input values you can use:
``` c
// Define low pass FIR coefficients in reverse order
const float32_t coef_C[] = { +0.68301f, +1.18301f, +0.31699f, -0.18301f };

// Define high pass FIR coefficients in reverse order
const float32_t coef_B[] = { -0.18301f, -0.31699f, +1.18301f, -0.68301f };

// Number of coefficients -- len(C) must be equal to len(B)
const size_t NC = sizeof(coef_C) / sizeof(float32_t);

const size_t N = 128;     // Number of samples provided as input for single run
float32_t in[N + NC - 1]; // Input values with extra NC - 1 samples
float32_t out[N];         // Buffer where N output values are stored

// Perform single run of FWT for prepared input values
arm_wt_f32_mallat(
  in,     // input data with extra samples depending on boundary condition
  out,    // output data aligned as in GNU Octave fwt function
  N,      // number of input values without extra ones
  2,      // number of decomposition iterations
  coef_C, // low pass FIR coefficients in reverse order
  coef_B, // high pass FIR coefficients in reverse order
  NC      // number of FIR coeficients for single filter
);

// Print output values
for (size_t i = 0; i < N; ++i) printf("%+5.5f\t", out[i]);
```

## Credits

- [Octave-FWT-Utils](https://github.com/Kuszki/Octave-FWT-Utils) -- GPL-3 -- some helpers for `GNU Octave`.
- [CMSIS-CORE](https://github.com/ARM-software/CMSIS_6) -- Apache 2.0 -- implementation of `CMSIS-CORE`.
- [CMSIS-DSP](https://github.com/ARM-software/CMSIS-DSP) -- Apache 2.0 -- implementation of `CMSIS-DSP`.

## License

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. 

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. 

You should have received a copy of the GNU General Public License along with this program. If not, see http://www.gnu.org/licenses/. 

