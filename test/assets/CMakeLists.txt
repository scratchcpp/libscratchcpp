# asset_test
add_executable(
  asset_test
  asset_test.cpp
  testasset.cpp
  testasset.h
)

target_link_libraries(
  asset_test
  GTest::gtest_main
  scratchcpp
)

gtest_discover_tests(asset_test)

# costume_test
add_executable(
  costume_test
  costume_test.cpp
)

target_link_libraries(
  costume_test
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp
  scratchcpp_mocks
)

gtest_discover_tests(costume_test)

# sound_test
add_executable(
  sound_test
  sound_test.cpp
)

target_link_libraries(
  sound_test
  GTest::gtest_main
  scratchcpp
)

gtest_discover_tests(sound_test)
