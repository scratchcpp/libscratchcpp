add_executable(
  load_project_test
  load_project_test.cpp
)

target_link_libraries(
  load_project_test
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp
  scratchcpp_mocks
)

gtest_discover_tests(load_project_test)
