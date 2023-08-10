# variable_blocks_test
add_executable(
  variable_blocks_test
  variable_blocks_test.cpp
)

target_link_libraries(
  variable_blocks_test
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp
  scratchcpp_mocks
)

gtest_discover_tests(variable_blocks_test)
