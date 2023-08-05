# ispritehandler_test
add_executable(
  ispritehandler_test
  ispritehandler_test.cpp
  testspriteinterface.cpp
  testspriteinterface.h
)

target_link_libraries(
  ispritehandler_test
  GTest::gtest_main
  scratchcpp
)

gtest_discover_tests(ispritehandler_test)

# istagehandler_test
add_executable(
  istagehandler_test
  istagehandler_test.cpp
  teststageinterface.cpp
  teststageinterface.h
)

target_link_libraries(
  istagehandler_test
  GTest::gtest_main
  scratchcpp
)

gtest_discover_tests(istagehandler_test)
