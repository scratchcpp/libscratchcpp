add_executable(
  cpp_test
  main.cpp
)

target_link_libraries(
  cpp_test
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp
  scratchcpp_mocks
)

gtest_discover_tests(cpp_test)
