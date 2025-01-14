add_executable(
  executioncontext_test
  executioncontext_test.cpp
)

target_link_libraries(
  executioncontext_test
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp
  scratchcpp_mocks
)

gtest_discover_tests(executioncontext_test)
