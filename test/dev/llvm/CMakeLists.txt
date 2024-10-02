add_executable(
  llvm_test
  llvmexecutioncontext_test.cpp
)

target_link_libraries(
  llvm_test
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp
  scratchcpp_mocks
)

gtest_discover_tests(llvm_test)
