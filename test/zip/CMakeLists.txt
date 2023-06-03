add_executable(
  zip_test
  zip_test.cpp
)

target_link_libraries(
  zip_test
  GTest::gtest_main
  zip
)

gtest_discover_tests(zip_test)
