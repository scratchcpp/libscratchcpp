add_executable(
  script_test
  script_test.cpp
)

target_link_libraries(
  script_test
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp
  scratchcpp_mocks
)

gtest_discover_tests(script_test)
