cmake_minimum_required(VERSION 3.14)

project(ARM-FWT-Test VERSION 0.1)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

include_directories(inc-dir)
enable_testing()

find_package(HDF5 REQUIRED COMPONENTS C HL)
add_subdirectory(cmsis-dsp/Source)

add_library(hdf-helper src-dir/hdf_helper.c inc-dir/hdf_helper.h)
target_include_directories(hdf-helper PRIVATE ${HDF5_INCLUDE_DIRS})
target_link_libraries(hdf-helper PUBLIC CMSISDSP ${HDF5_LIBRARIES})

add_library(mat-helper src-dir/mat_helper.c inc-dir/mat_helper.h)
target_link_libraries(mat-helper PUBLIC CMSISDSP m)

add_executable(basetest test-dir/base.c)
target_link_libraries(basetest PUBLIC CMSISDSP m)
add_test(NAME test-base COMMAND basetest)

add_executable(loadtest test-dir/load.c)
target_link_libraries(loadtest PUBLIC hdf-helper)
add_test(NAME test-load COMMAND loadtest)

add_executable(multest test-dir/matmul.c)
target_link_libraries(multest PUBLIC CMSISDSP mat-helper hdf-helper)
add_test(NAME test-matmul COMMAND multest)

add_executable(cmptest test-dir/compare.c)
target_link_libraries(cmptest PUBLIC mat-helper)
add_test(NAME test-compare COMMAND cmptest)

execute_process(COMMAND octave gen_multest.m	WORKING_DIRECTORY ${PROJECT_DIR}/src-dir)
file(COPY data-dir/mul_out.hdf DESTINATION ${PROJECT_BINARY_DIR})
