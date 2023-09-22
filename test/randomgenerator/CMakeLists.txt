add_executable(
  randomgenerator_test
  randomgenerator_test.cpp
)

target_link_libraries(
  randomgenerator_test
  GTest::gtest_main
  scratchcpp
)

gtest_discover_tests(randomgenerator_test)
