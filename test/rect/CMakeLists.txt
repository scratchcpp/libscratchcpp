add_executable(
  rect_test
  rect_test.cpp
)

target_link_libraries(
  rect_test
  GTest::gtest_main
  scratchcpp
)

gtest_discover_tests(rect_test)
