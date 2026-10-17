add_library(
  llvm_test_functions SHARED
  testfunctions.cpp
  testfunctions.h
  testmock.h
)

target_link_libraries(
  llvm_test_functions
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp
  scratchcpp_mocks
)

add_executable(
  llvm_test
  main.cpp
  llvmexecutioncontext_test.cpp
  llvmexecutablecode_test.cpp
  llvminstructionlist_test.cpp
  code_analyzer/variable_type_analysis.cpp
  code_analyzer/list_type_analysis.cpp
  code_analyzer/mixed_type_analysis.cpp
)

target_link_libraries(
  llvm_test
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp
  scratchcpp_mocks
  llvm_test_functions
  LLVM
)

gtest_discover_tests(llvm_test)
