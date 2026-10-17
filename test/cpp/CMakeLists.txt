# Test cases are shared by the generator and the test
add_library(
  cpp_test_cases STATIC
  cases.cpp
  cases.h
)

target_link_libraries(cpp_test_cases scratchcpp)

# The generator writes the C++ code of the test cases at build time
add_executable(
  cpp_test_generator
  generator.cpp
)

target_link_libraries(cpp_test_generator cpp_test_cases scratchcpp)

set(CPP_TEST_GENERATED ${CMAKE_CURRENT_BINARY_DIR}/generated_cases.cpp)

add_custom_command(
  OUTPUT ${CPP_TEST_GENERATED}
  COMMAND cpp_test_generator ${CPP_TEST_GENERATED}
  DEPENDS cpp_test_generator
  COMMENT "Generating C++ code of the C++ code builder test cases"
)

add_executable(
  cpp_test
  cpp_test.cpp
  testfunctions.cpp
  ${CPP_TEST_GENERATED}
)

target_link_libraries(
  cpp_test
  GTest::gtest_main
  cpp_test_cases
  scratchcpp
)

gtest_discover_tests(cpp_test)
