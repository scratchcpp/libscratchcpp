add_executable(
  engine_test
  engine_test.cpp
  testsection.cpp
  testsection.h
)

target_link_libraries(
  engine_test
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp
  scratchcpp_mocks
)

gtest_discover_tests(engine_test)
