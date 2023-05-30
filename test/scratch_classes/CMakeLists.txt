# blockprototype_test
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

# list_test
add_executable(
  list_test
  list_test.cpp
)

target_link_libraries(
  list_test
  GTest::gtest_main
  scratchcpp
)

gtest_discover_tests(list_test)
