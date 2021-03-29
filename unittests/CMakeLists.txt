enable_testing()

# Add doctest
FetchContent_Declare(
  doctest
  GIT_REPOSITORY https://github.com/onqtam/doctest.git
  GIT_TAG 2.4.4)

set(DOCTEST_WITH_TESTS OFF)
FetchContent_MakeAvailable(doctest)

include_directories("${doctest_SOURCE_DIR}")
include_directories("../src/")

set(TARGETS "legendre_test" "stream_test")
foreach(target ${TARGETS})
  add_executable(${target} ${target}.cpp)
  target_link_libraries(${target} code-coverage-demo)
  target_code_coverage(${target} AUTO ALL)
  add_test(${target} ./${target})
endforeach()

