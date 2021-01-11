# How to support code coverage report in your C++ in 3 steps

# Introduction #

Consider a typical layout of a C++ project as below

``` text

```

where 
* There a CMakeLists.txt file at the root folder.
* The **src** folder stores all source code and these source files will be compiled into a static library named **libcode-coverage-demo.a**
* All test files are in the unittests folder.
* The **_deps** folder has all of our required libraries. In this demo we use doctest and Eigen libraries.

Note: We will use LLVM toolchain in our examples, however, keep in mind that GCC toolchain also work.




# Steps #

Note: This guide only works on Linux and macOS.

## Step 1: Update your CMakeLists.txt ##

### Add required CMake scripts to your CMakeLists.txt ###

### Add the code coverage support to your CMakeLists.txt ###




## Step 2: Configure the CMake build ##

``` shell
cmake ./ -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=clang++ -DCODE_COVERAGE=ON
```

## Step 3: Generate the code coverage report ##

``` shell
make ccov-all
make ccov-all-report
```

Below is the sample code coverage report

``` shell
Filename                          Regions    Missed Regions     Cover   Functions  Missed Functions  Executed       Lines      Missed Lines     Cover
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
src/encoding/legendre.cpp              13                 0   100.00%           2                 0   100.00%          25                 0   100.00%
src/stream.hpp                          7                 0   100.00%           2                 0   100.00%          13                 0   100.00%
unittests/doctest-helpers.hpp          24                 4    83.33%           2                 0   100.00%          22                 4    81.82%
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
TOTAL                                  44                 4    90.91%           6                 0   100.00%          60                 4    93.33%
```

The html report can be found from the **ccov** folder.
