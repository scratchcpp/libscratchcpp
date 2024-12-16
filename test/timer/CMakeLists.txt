add_executable(
  timer_test
  timer_test.cpp
  stacktimer_test.cpp
)

target_link_libraries(
  timer_test
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp
  scratchcpp_mocks
)

gtest_discover_tests(timer_test)
