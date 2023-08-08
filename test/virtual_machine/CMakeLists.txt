add_executable(
  virtual_machine_test
  virtual_machine_test.cpp
)

target_link_libraries(
  virtual_machine_test
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp
  scratchcpp_mocks
)

gtest_discover_tests(virtual_machine_test)
