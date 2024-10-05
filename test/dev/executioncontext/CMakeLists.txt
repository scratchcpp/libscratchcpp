add_executable(
  executioncontext_test
  executioncontext_test.cpp
)

target_link_libraries(
  executioncontext_test
  GTest::gtest_main
  scratchcpp
)

gtest_discover_tests(executioncontext_test)
