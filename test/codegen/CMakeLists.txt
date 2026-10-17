add_library(
  codegen_test_functions SHARED
  testfunctions.cpp
  testfunctions.h
  testmock.h
)

target_link_libraries(
  codegen_test_functions
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp
  scratchcpp_mocks
)

add_executable(
  codegen_test
  main.cpp
  codebuilder_test.cpp
  codegentestutils.cpp
  codegentestutils.h
)

target_link_libraries(
  codegen_test
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp
  scratchcpp_mocks
  codegen_test_functions
)

gtest_discover_tests(codegen_test)
