add_executable(
  promise_test
  promise_test.cpp
)

target_link_libraries(
  promise_test
  GTest::gtest_main
  scratchcpp
)

gtest_discover_tests(promise_test)
