#!/bin/bash
cmake ./ -DCMAKE_BUILD_TYPE=Debug -DCODE_COVERAGE=ON -DCMAKE_CXX_COMPILER=clang++
make -j3
make ccov-all -j3
make ccov-all-report
