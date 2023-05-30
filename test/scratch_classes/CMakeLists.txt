add_executable(
  blockprototype_test
  blockprototype_test.cpp
)

target_link_libraries(
  blockprototype_test
  GTest::gtest_main
  scratchcpp
)

gtest_discover_tests(blockprototype_test)
