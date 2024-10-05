add_executable(
  compiler_test
  compiler_test.cpp
)

target_link_libraries(
  compiler_test
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp
  scratchcpp_mocks
)

gtest_discover_tests(compiler_test)
