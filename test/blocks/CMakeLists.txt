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

# list_blocks_test
add_executable(
  list_blocks_test
  list_blocks_test.cpp
)

target_link_libraries(
  list_blocks_test
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp
  scratchcpp_mocks
)

gtest_discover_tests(list_blocks_test)

# operator_blocks_test
add_executable(
  operator_blocks_test
  operator_blocks_test.cpp
)

target_link_libraries(
  operator_blocks_test
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp
  scratchcpp_mocks
)

gtest_discover_tests(operator_blocks_test)

# event_blocks_test
add_executable(
  event_blocks_test
  event_blocks_test.cpp
)

target_link_libraries(
  event_blocks_test
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp
  scratchcpp_mocks
)

gtest_discover_tests(event_blocks_test)

# control_blocks_test
add_executable(
  control_blocks_test
  control_blocks_test.cpp
)

target_link_libraries(
  control_blocks_test
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp
  scratchcpp_mocks
)

gtest_discover_tests(control_blocks_test)
