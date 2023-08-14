add_executable(
  asset_test
  asset_test.cpp
)

target_link_libraries(
  asset_test
  GTest::gtest_main
  scratchcpp
)

gtest_discover_tests(asset_test)
