#!/bin/bash

cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_GENERATOR=Ninja -S . -B build-dir
cmake --build build-dir
ctest --test-dir build-dir
