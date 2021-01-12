# How to support code coverage report in your C++ in 3 steps

# Introduction #

Code coverage is an important metrics to measure our code quality. This blog post will 
* Explain what is code coverage and how we use it.
* Explain in detail how we do code coverage for a C++ project.
* Demonstrate that we can support code coverage in Tableau C++ projects in 3 **simple** steps.

# What is code coverage #

Code coverage analysis is a measurement that is taken when running program that is taken on running program code reporting how much of that code was executed. Specifically, code coverage usually tells you how many times each line of code was executed. There are various forms of coverage metrics, such as function coverage, sequence point coverage, statement coverage and branch coverage. I’ll elaborate on some of these in later posts. For now, NCover supports sequence point (which is basically statement coverage) and branch coverage. 

# Why do we need code coverage #

Code coverage is most valuable as a feedback mechanism for test-driven or agile development methodologies. Both of these methods rely on a developmental feedback loop that promotes the addition of features while maintaining a predictable quality level. Code quality is one of the most important concerns of any software development organization. Rapid development and technical wizardry will do you no good if customers can’t rely on the quality of your product.

The way most modern software development teams maintain the quality of their software is by building a suite of unit tests that automate the verification of code correctness. These unit test suites are designed to run through (often thousands) of simulated program functions, verifying that the code is producing the expected output. While creating a number of unit tests is laudable, just creating a suite of tests alone is not enough. After some point, adding more tests does not necessarily mean that the code has better quality. You may just be testing the same code over and over again, while missing an important piece of code entirely. Any bugs in code that is not tested will not be detected by your unit test suite and thus could creep into production.

Coverage analysis closes that feedback loop by reporting on the comprehensiveness of your unit tests. Code coverage reports allow developers to quickly find code that is not executed by the test suite.

# When should we use code coverage #

Early and Often…is the short answer. The heart of an iterative development environment is a continuous integration server. Each time a developer checks code into your revision control system (or on a regularly scheduled cycle), the build server should check out the latest code, do a full build of the product, run the test suite and report test success/failure and code coverage metrics. A continuous build process facilitates the early detection of code errors. Immediate reporting of code coverage also makes it clear to everyone on the team if the quality of your tests are increasing or declining.

# The code coverage workflow #

The code coverage workflow has three steps:
1. Compiling our code with coverage enabled.
2. Running the instrumented program i.e unit and integration tests.
3. Generating the coverage reports.

Let consider [a simple example](https://clang.llvm.org/docs/SourceBasedCodeCoverage.html) from the LLVM project 

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

## Compling our code with coverage enabled ##

``` shell
clang++ -fprofile-instr-generate -fcoverage-mapping demo.cpp -o demo
```

## Running the instrumented program ##

``` shell
LLVM_PROFILE_FILE="demo.profraw" ./demo
```

## Creating code coverage report ##

All raw profiles have to be indexed before they can be used to generate our coverage reports. This task can be done using llvm-profdata command.

``` shell
llvm-profdata merge -sparse demo.profraw -o demo.profdata
```

After all raw profile data has been indexed, we can use it to generate the code coverage reports. There are two types of code coverage report:

**The line-oriented code coverage report**

```
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

llvm-cov provides several options to show different branch covergage information. For example if the `` flag is set we can generate a below report

``` c++
hungdang-ltm:demo hung.dang$ llvm-cov show --show-expansions ./demo -instr-profile=demo.profdata
    1|     20|#define BAR(x) ((x) || (x))
    2|       |
    3|      2|template <typename T> void foo(T x) {
    4|     22|    for (unsigned I = 0; I < 10; ++I) { BAR(I); }
  ------------------
  |  |    1|     10|#define BAR(x) ((x) || (x))
  ------------------
                  for (unsigned I = 0; I < 10; ++I) { BAR(I); }
  ------------------
  |  |    1|     10|#define BAR(x) ((x) || (x))
  ------------------
    5|      2|}
  ------------------
  | _Z3fooIiEvT_:
  |    3|      1|template <typename T> void foo(T x) {
  |    4|     11|    for (unsigned I = 0; I < 10; ++I) { BAR(I); }
  |  ------------------
  |  |  |    1|     10|#define BAR(x) ((x) || (x))
  |  ------------------
  |    5|      1|}
  ------------------
  | _Z3fooIfEvT_:
  |    3|      1|template <typename T> void foo(T x) {
  |    4|     11|    for (unsigned I = 0; I < 10; ++I) { BAR(I); }
  |  ------------------
  |  |  |    1|     10|#define BAR(x) ((x) || (x))
  |  ------------------
  |    5|      1|}
  ------------------
    6|       |
    7|      1|int main() {
    8|      1|    foo<int>(0);
    9|      1|    foo<float>(0);
   10|      1|    return 0;
   11|      1|}
```

**The file level code coverage report**

Often we want to see the high level information of the code coverage. We can do it using the `-instr-profile` option

```
hungdang-ltm:demo hung.dang$ llvm-cov report ./demo -instr-profile=demo.profdata
Filename                                                      Regions    Missed Regions     Cover   Functions  Missed Functions  Executed       Lines      Missed Lines     Cover
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/Users/hung.dang/working/code-coverage-demo/demo/demo.cpp           8                 0   100.00%           2                 0   100.00%           8                 0   100.00%
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
TOTAL                                                               8                 0   100.00%           2                 0   100.00%           8                 0   100.00%
```

# How to add code coverage support for our C++ project #

Note: This guide only works on Linux and macOS.

At this point, we know how to generate the code coverage report for a single executable. However, in real C++ projects there are many executable binaries and the list of our binaries are changing frequently so the above approach does not scale by time. Luckily, we can automate the above process using CMake.

Consider a typical layout of [a C\+\+ project](https://github.com/hungptit/code-coverage-demo) as below

``` text
.
├── CMakeLists.txt
├── LICENSE
├── README.md
├── src
│   ├── CMakeLists.txt
│   ├── encoding
│   │   ├── legendre.cpp
│   │   └── legendre.hpp
│   └── stream.hpp
└── unittests
    ├── CMakeLists.txt
    ├── doctest-helpers.hpp
    ├── legendre_test.cpp
    └── stream_test.cpp
```

where 
* There is a CMakeLists.txt file at the root folder that handles below tasks
  * Downloading all dependencies for our project i.e [doctest](https://github.com/onqtam/doctest) and [Eigen](https://gitlab.com/libeigen/eigen).
  * Formating and linting all CMake and C++ files automatically using cmake-format, cmake-lint, and clang-format.
  * Create a static librqary named **libcode-coverage-demo.a**
  * Compile and run all unit tests.
* The **src** folder stores all source code and these source files will be compiled into a static library named **libcode-coverage-demo.a**
* All test files are in the unittests folder.
* The **_deps** folder, which is not shown in our example, has all of our required libraries. In this demo we use doctest and Eigen libraries.

Note: We will use LLVM toolchain in our examples, however, keep in mind that GCC toolchain also work.

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
