add_executable(
  project_test
  project_test.cpp
)

target_link_libraries(
  project_test
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp
  scratchcpp_mocks
)

gtest_discover_tests(project_test)
