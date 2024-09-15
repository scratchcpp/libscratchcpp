add_executable(
  thread_test
  thread_test.cpp
)

target_link_libraries(
  thread_test
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp
  scratchcpp_mocks
)

gtest_discover_tests(thread_test)
