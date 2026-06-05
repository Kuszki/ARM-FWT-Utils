cmake_minimum_required(VERSION 3.21)

project(ARM-FWT-Test VERSION 0.1)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

include_directories(inc-dir)
enable_testing()

find_program(OCTAVE_EXECUTABLE octave REQUIRED)

find_package(HDF5 REQUIRED COMPONENTS C HL)
add_subdirectory(cmsis-dsp/Source)

add_library(helper-hdf
	src-dir/hdf_helper.c
	inc-dir/hdf_helper.h)
target_include_directories(helper-hdf PRIVATE ${HDF5_INCLUDE_DIRS})
target_link_libraries(helper-hdf PUBLIC CMSISDSP ${HDF5_LIBRARIES})

add_library(helper-mat
	src-dir/mat_helper.c
	inc-dir/mat_helper.h)
target_link_libraries(helper-mat PUBLIC CMSISDSP m)

add_library(lib-fwt
	src-dir/arm_mal_f32.c
	inc-dir/arm_mal_f32.h
	src-dir/arm_wt_f32.c
	inc-dir/arm_wt_f32.h
	src-dir/arm_wt_q15.c
	inc-dir/arm_wt_q15.h
	inc-dir/arm_wt_common.h)
target_link_libraries(lib-fwt PUBLIC CMSISDSP m)

add_executable(test-base test-dir/base.c)
target_link_libraries(test-base PUBLIC CMSISDSP m)
add_test(NAME test-base COMMAND test-base)

add_executable(test-load test-dir/load.c)
target_link_libraries(test-load PUBLIC helper-hdf)
add_test(NAME test-load COMMAND test-load)

add_executable(test-mul test-dir/matmul.c)
target_link_libraries(test-mul PUBLIC CMSISDSP helper-mat helper-hdf)
add_test(NAME test-matmul COMMAND test-mul)

add_executable(test-cmp test-dir/compare.c)
target_link_libraries(test-cmp PUBLIC helper-mat)
add_test(NAME test-compare COMMAND test-cmp)

add_executable(test-coe test-dir/coefs.c)
target_link_libraries(test-coe PUBLIC helper-mat helper-hdf lib-fwt)
add_test(NAME test-coefs COMMAND test-coe)

add_executable(test-mall test-dir/mallat.c)
target_link_libraries(test-mall PUBLIC helper-mat helper-hdf lib-fwt)
add_test(NAME test-mallat COMMAND test-mall)

add_executable(test-cwt test-dir/armcwt.c)
target_link_libraries(test-cwt PUBLIC helper-mat helper-hdf lib-fwt)
add_test(NAME test-armcwt COMMAND test-cwt)

add_executable(test-fwt test-dir/armfwt.c)
target_link_libraries(test-fwt PUBLIC helper-mat helper-hdf lib-fwt)
add_test(NAME test-armfwt COMMAND test-fwt)

add_executable(test-qwt test-dir/armqwt.c)
target_link_libraries(test-qwt PUBLIC helper-mat helper-hdf lib-fwt CMSISDSP)
add_test(NAME test-armqwt COMMAND test-qwt)

execute_process(
	COMMAND ${Octave_EXECUTABLE} gen_multest.m
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/src-dir)
execute_process(
	COMMAND ${Octave_EXECUTABLE} gen_fwtmat.m
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/src-dir)
execute_process(
	COMMAND ${Octave_EXECUTABLE} gen_mallat.m
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/src-dir)
execute_process(
	COMMAND ${Octave_EXECUTABLE} gen_firtest.m
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/src-dir)

file(GLOB HDF_SAVED_VARIABLES "${PROJECT_SOURCE_DIR}/data-dir/*.hdf")
file(COPY ${HDF_SAVED_VARIABLES} DESTINATION ${PROJECT_BINARY_DIR})
