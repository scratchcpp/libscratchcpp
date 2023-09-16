add_executable(
  clock_test
  clock_test.cpp
)

target_link_libraries(
  clock_test
  GTest::gtest_main
  scratchcpp
)

gtest_discover_tests(clock_test)
