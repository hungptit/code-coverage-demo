# How to generate the code coverage report for your CMake C++ projects

## What is code coverage? ##

Source-based code coverage is a measure used to describe the degree to which the source code of a program is executed when a particular test suite runs. There are various forms of coverage metrics, such as function coverage, line coverage, statement coverage, and branch coverage. We will cover all types of source-based code coverage supported by the LLVM and GCC tool-chains in our demos.

## Why do we need code coverage? ##

There are many reasons to have code coverage. Below are a few important benefits of having code coverage measures:
* To know how well our automated tests actually test our code.
* To maintain our test quality over the lifecycle of projects.

## The code coverage workflow ##

The code coverage workflow has three steps:

1. Compiling our code with coverage enabled.
2. Running the instrumented programs i.e unit and integration tests.
3. Generating coverage reports.

Let's consider a simple example from the LLVM project

``` c++
#define BAR(x) ((x) || (x))

template <typename T> void foo(T x) {
    for (unsigned I = 0; I < 10; ++I) { BAR(I); }
}

int main() {
    foo<int>(0);
    foo<float>(0);
    return 0;
}
```

We fist need compile our demo with coverage enabled using clang++

``` shell
clang++ -fprofile-instr-generate -fcoverage-mapping demo.cpp -o demo
```

Next, we execute our demo program to generate coverage data

``` shell
LLVM_PROFILE_FILE="demo.profraw" ./demo
```

At this point, all source-based coverage information has been generated and we need to merge all raw profiles before generating the coverage report. This task can be done using `llvm-profdata` command

``` shell
llvm-profdata merge -sparse demo.profraw -o demo.profdata
```

Now we can generate the line-oriented and file level coverage reports for our demo

``` text
hungdang-ltm:demo hung.dang$ llvm-cov show ./demo -instr-profile=demo.profdata
    1|     20|#define BAR(x) ((x) || (x))
    2|       |
    3|      2|template <typename T> void foo(T x) {
    4|     22|    for (unsigned I = 0; I < 10; ++I) { BAR(I); }
    5|      2|}
  ------------------
  | _Z3fooIiEvT_:
  |    3|      1|template <typename T> void foo(T x) {
  |    4|     11|    for (unsigned I = 0; I < 10; ++I) { BAR(I); }
  |    5|      1|}
  ------------------
  | _Z3fooIfEvT_:
  |    3|      1|template <typename T> void foo(T x) {
  |    4|     11|    for (unsigned I = 0; I < 10; ++I) { BAR(I); }
  |    5|      1|}
  ------------------
    6|       |
    7|      1|int main() {
    8|      1|    foo<int>(0);
    9|      1|    foo<float>(0);
   10|      1|    return 0;
   11|      1|}
```

``` text
hungdang-ltm:demo hung.dang$ llvm-cov report ./demo -instr-profile=demo.profdata
Filename                                                      Regions    Missed Regions     Cover   Functions  Missed Functions  Executed       Lines      Missed Lines     Cover
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/Users/hung.dang/working/code-coverage-demo/demo/demo.cpp           8                 0   100.00%           2                 0   100.00%           8                 0   100.00%
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
TOTAL                                                               8                 0   100.00%           2                 0   100.00%           8                 0   100.00%
```

## How can we add code coverage support for a CMake C++ project? ##

The above example shows that we can generate both line and file level coverage reports, however, there are several drawbacks when applying this approach in real C++ projects
* The code coverage data is generated outside of the build process.
* The code coverage collection process is executed manually thus does not scale by the number of C++ files.

Luckily, there are efforts in the open-source space to add code coverage support for C++ projects using either LLVM or GCC tool-chains. Our demo project will use [cmake-scripts](https://github.com/StableCoder/cmake-scripts) to automate all code coverage generation steps.

Let's consider a typical CMake C++ project with the below layout

``` text
./
├── CMakeLists.txt
├── LICENSE
├── README.md
├── demo
│   ├── demo.cpp
├── src
│   ├── CMakeLists.txt
│   ├── encoding
│   │   ├── legendre.cpp
│   │   └── legendre.hpp
│   └── stream.hpp
└── unittests
    ├── CMakeLists.txt
    ├── doctest-helpers.hpp
    ├── legendre_test.cpp
    └── stream_test.cpp
```

where
* The root-level CMakeLists.txt will handle all of heavy work including downloading dependencies, formatting code, generating code-coverage-demo library, compiling and executing all unit tests.
* The src folder store our source code i.e hpp and cpp files.
* The unittests folder stores all test files.

Adding code coverage support for the demo CMake project can be divided into 3 steps

### Step 1: Add our library to the code coverage build ###

``` text
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS encoding/*.cpp)
add_library(code-coverage-demo STATIC ${SRC_FILES})

# This line add code-coverage-demo library to the code coverage build
target_code_coverage(code-coverage-demo AUTO ALL)
```

### Step 2: Add test files to the code coverage build ###

``` text
set(TEST_EXECUTABLE legendre_test stream_test)
foreach(src_file ${SRC_FILES})
  add_executable(${src_file} ${src_file}.cpp)
  target_link_libraries(${src_file} code-coverage-demo)
  target_code_coverage(${src_file} AUTO ALL) # Add all test files to the code coverage build
  add_test(${src_file} ./${src_file})
endforeach(src_file)
```

### Step 3: Update the root level CMakeLists.txt and generate the code coverage report ###

Most heavy work will be done in this step including:
* Tell CMake to download cmake-scripts and store it in _deps folder.
* Include the code-coverage.cmake file.
* Enable code coverage support for all sub-folders and exclude test files i.e _test.cpp from the coverage report.

``` text
# Add cmake-scripts
FetchContent_Declare(
  cmake_scripts
  GIT_REPOSITORY https://github.com/StableCoder/cmake-scripts.git
  GIT_TAG main
  GIT_SHALLOW TRUE)

FetchContent_Populate(cmake_scripts)

include(code-coverage)

# Exclude test files from the code coverage report.
file(GLOB_RECURSE TEST_SRCS unittests/*_test.cpp)
add_code_coverage_all_targets(EXCLUDE _deps/ ${TEST_SRCS})
```

We have made all required changes to support the code coverage collection for our project. Let's build our project with the updated CMakeLists.txt and see how the code coverage report looks like

``` shell
cmake . -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=clang++ -DCODE_COVERAGE=ON
make ccov-all
make ccov-all-report
```

Below is the sample text-based code coverage report

``` text
Filename                          Regions    Missed Regions     Cover   Functions  Missed Functions  Executed       Lines      Missed Lines     Cover
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
src/encoding/legendre.cpp              13                 0   100.00%           2                 0   100.00%          25                 0   100.00%
src/stream.hpp                          7                 0   100.00%           2                 0   100.00%          13                 0   100.00%
unittests/doctest-helpers.hpp          24                 4    83.33%           2                 0   100.00%          22                 4    81.82%
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
TOTAL                                  44                 4    90.91%           6                 0   100.00%          60                 4    93.33%
```

The text-based reports work fine for our purpose, however, we can also open the HTML reports in the `ccov/all-merged/` folder which allow us navigating through coverage reports in our favourist web browser.

That's all folks. Supporting code coverage analysis is quite simple for any CMake C++ project. Let us (Hung Dang, Khoi Nguyen, and Huyen Nguyen) know your thoughts.

## FAQs ##

### How can I build the demo? ###

Run below commands in any Linux/macOS terminal to generate the code coverage report for our demo, assuming LLVM toolchain has been installed in the search path.

``` shell
git clone https://github.com/hungptit/code-coverage-demo.git
cd code-coverage-demo
./build_all.sh
```
