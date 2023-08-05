# ispritehandler_test
add_executable(
  ispritehandler_test
  ispritehandler_test.cpp
)

target_link_libraries(
  ispritehandler_test
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp
  scratchcpp_mocks
)

gtest_discover_tests(ispritehandler_test)

# istagehandler_test
add_executable(
  istagehandler_test
  istagehandler_test.cpp
)

target_link_libraries(
  istagehandler_test
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp
  scratchcpp_mocks
)

gtest_discover_tests(istagehandler_test)
