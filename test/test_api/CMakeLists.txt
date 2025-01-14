add_library(
  test_api_test_deps SHARED
  testextension.cpp
  testextension.h
)

target_link_libraries(
  test_api_test_deps
  GTest::gtest_main
  scratchcpp
)

add_executable(
  test_api_test
  scriptbuilder_test.cpp
)

target_link_libraries(
  test_api_test
  GTest::gtest_main
  scratchcpp
  test_api_test_deps
)

gtest_discover_tests(test_api_test)
