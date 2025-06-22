#!/bin/bash

cmake -DCMAKE_GENERATOR=Ninja -S . -B build-dir
cmake --build build-dir
ctest --test-dir build-dir
