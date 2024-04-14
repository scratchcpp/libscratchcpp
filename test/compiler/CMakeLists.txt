add_executable(
  compiler_test
  compiler_test.cpp
  testblocksection.cpp
  testblocksection.h
)

target_link_libraries(
  compiler_test
  GTest::gtest_main
  scratchcpp
  nlohmann_json::nlohmann_json
)

gtest_discover_tests(compiler_test)
