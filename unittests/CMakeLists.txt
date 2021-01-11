include_directories("../_deps/doctest-src")
include_directories("../src/")

set(SRC_FILES legendre_test stream_test)
foreach(src_file ${SRC_FILES})
  add_executable(${src_file} ${src_file}.cpp)
  target_link_libraries(${src_file} code-coverage-demo)
  target_code_coverage(${src_file} AUTO ALL)
  add_test(${src_file} ./${src_file})
endforeach(src_file)

enable_testing()
