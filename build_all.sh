#!/bin/bash -ex

function build {
    cmake . "$@"
    cmake --build . -j3
    ctest -j3

    # Format all files
    make format
    make lint
    
    # Generate coverage report
    make ccov-all
    make ccov-all-report
}

# Generate code coverage report
build -DCODE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=clang++
