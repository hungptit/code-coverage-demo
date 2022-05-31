#!/bin/bash -ex
root_dir="$PWD"

function build {
    cmake . -DCODE_COVERAGE=ON "$@"
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
build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=clang++
