cmake_policy(SET CMP0048 NEW)
project(
  code-coverage-demo
  VERSION 0.1.0
  LANGUAGES CXX)
cmake_minimum_required(VERSION 3.15)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Use C++ 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Adjust compiler flags
add_compile_options(-static)
add_compile_options(-pedantic)
add_compile_options(-pedantic-errors)
set(POSITION_INDEPENDENT_CODE ON)
set(ENABLE_ALL_WARNINGS ON)

# Enable the Makefile verbose mode to show verbose build information in macOS or
# Linux.
set(CMAKE_VERBOSE_MAKEFILE OFF)

include(FetchContent)

# Add doctest
FetchContent_Declare(
  doctest
  GIT_REPOSITORY https://github.com/onqtam/doctest.git
  GIT_TAG 2.4.4)

set(DOCTEST_WITH_TESTS OFF)
FetchContent_MakeAvailable(doctest)

# Eigen
FetchContent_Declare(
  eigen
  GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
  GIT_TAG master)
set(BUILD_TESTING OFF)
FetchContent_Populate(eigen)

# Add cmake-scripts
FetchContent_Declare(
  cmake_scripts
  GIT_REPOSITORY https://github.com/StableCoder/cmake-scripts.git
  GIT_TAG 20.11)

FetchContent_Populate(cmake_scripts)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/_deps/cmake_scripts-src/;")
include(c++-standards)
include(code-coverage)
include(sanitizers)
include(tools)
include(dependency-graph)

# Exclude test files from the code coverage report.
file(GLOB_RECURSE TEST_SRCS CONFIGURE_DEPENDS unittests/*_test.cpp)
add_code_coverage_all_targets(EXCLUDE _deps/ ${TEST_SRCS})

# Include folders
include_directories("_deps/eigen-src/")

# Add sub-directories
add_subdirectory(src)
add_subdirectory(unittests)

# Formating and linting all related files
file(
  GLOB_RECURSE
  SRCS_AND_HDRS
  CONFIGURE_DEPENDS
  unittests/*.[hc]pp
  unittests/*.hpp
  src/*.hpp
  src/encoding/*.[hc]pp)

find_program(CLANG_FORMAT NAMES clang-format)
if(${CLANG_FORMAT} STREQUAL "CLANG_FORMAT-NOTFOUND")
  add_custom_command(
    OUTPUT .format_cpp_stamp
    DEPENDS ${SRCS_AND_HDRS}
    COMMAND touch .format_cpp_stamp
    COMMENT "Could not find clang-format. Skip formating C++ files."
    VERBATIM)
else()
  add_custom_command(
    OUTPUT .format_cpp_stamp
    DEPENDS ${SRCS_AND_HDRS}
    COMMAND ${CLANG_FORMAT} -i ${SRCS_AND_HDRS}
    COMMAND touch .format_cpp_stamp
    COMMENT "Format C++ files with clang-format."
    VERBATIM)
endif()

set(CMAKE_FILES "CMakeLists.txt" "src/CMakeLists.txt")
find_program(CMAKE_FORMAT NAMES cmake-format)
if(${CMAKE_FORMAT} STREQUAL "CMAKE_FORMAT-NOTFOUND")
  add_custom_command(
    OUTPUT .format_cmake_stamp
    DEPENDS ${CMAKE_FILES}
    COMMAND touch .format_cmake_stamp
    COMMENT "Skip formatting CMake files."
    VERBATIM)
else()
  add_custom_command(
    OUTPUT .format_cmake_stamp
    DEPENDS ${CMAKE_FILES}
    COMMAND cmake-format -i ${CMAKE_FILES}
    COMMAND touch .format_cmake_stamp
    COMMENT "Format CMake files with cmake-format."
    VERBATIM)
endif()

file(GLOB SH_SRCS 3p/*.sh)
find_program(SHFORMAT NAMES shformat)
if(${SHFORMAT} STREQUAL "SHFORMAT-NOTFOUND")
  add_custom_command(
    OUTPUT .format_bash_stamp
    DEPENDS ${SH_SRCS}
    COMMAND touch .format_bash_stamp
    COMMENT "Skip formatting Bash scripts."
    VERBATIM)
else()
  add_custom_command(
    OUTPUT .format_bash_stamp
    DEPENDS ${SH_SRCS}
    COMMAND ${SHFORMAT} -i 2 -ci -w ${SH_SRCS}
    COMMAND touch .format_bash_stamp
    COMMENT "Format Bash scripts with shformat."
    VERBATIM)
endif()

add_custom_target(
  format
  DEPENDS .format_cpp_stamp .format_cpp_stamp .format_cmake_stamp
  COMMENT "Format Bash, C++, and CMake files.")

find_program(SHELLCHECK NAMES shellcheck)
if(${SHELLCHECK} STREQUAL "SHELLCHECK-NOTFOUND")
  add_custom_command(
    OUTPUT .lint_shellcheck_stamp
    DEPENDS ${SH_SRCS}
    COMMAND touch .lint_shellcheck_stamp
    COMMENT "Skip linting Bash scripts."
    VERBATIM)
else()
  add_custom_command(
    OUTPUT .lint_shellcheck_stamp
    DEPENDS ${SH_SRCS}
    COMMAND shellcheck ${SH_SRCS}
    COMMAND touch .lint_shellcheck_stamp
    COMMENT "Lint Bash files with shellcheck.")
endif()

find_program(CMAKE_LINT NAMES cmake-lint)
if(${CMAKE_LINT} STREQUAL "CMAKE_LINT-NOTFOUND")
  add_custom_command(
    OUTPUT .lint_cmake_stamp
    DEPENDS ${CMAKE_FILES}
    COMMAND echo "Skip linting CMake files."
    COMMAND touch .lint_cmake_stamp
    COMMENT "Skip linting CMake files.")
else()
  add_custom_command(
    OUTPUT .lint_cmake_stamp
    DEPENDS ${CMAKE_FILES}
    COMMAND cmake-lint ${CMAKE_FILES}
    COMMAND touch .lint_cmake_stamp
    COMMENT "Lint CMake files with cmake-lint.")
endif()

add_custom_target(
  lint ALL
  DEPENDS .lint_cmake_stamp .lint_shellcheck_stamp
  COMMENT "Lint Bash and CMake files.")

enable_testing()
