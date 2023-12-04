# projecturl_test
add_executable(
  projecturl_test
  projecturl_test.cpp
)

target_link_libraries(
  projecturl_test
  GTest::gtest_main
  scratchcpp
)

gtest_discover_tests(projecturl_test)
