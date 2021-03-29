file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS encoding/*.cpp)
add_library(code-coverage-demo STATIC ${SRC_FILES})
target_code_coverage(code-coverage-demo AUTO ALL)
