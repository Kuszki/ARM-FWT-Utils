cmake_minimum_required(VERSION 3.14)

project(ARM-FWT-Test VERSION 0.1)

enable_testing()

include_directories(inc-dir)

add_subdirectory(cmsis-dsp/Source)
add_executable(basetest test-dir/base.c)
add_test(NAME test-base COMMAND basetest)

target_link_libraries(basetest PUBLIC CMSISDSP m)
