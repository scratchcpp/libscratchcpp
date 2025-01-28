add_executable(
  compiler_test
  compiler_test.cpp
  testextension.cpp
  testextension.h
)

target_link_libraries(
  compiler_test
  GTest::gtest_main
  scratchcpp
  zip
  nlohmann_json::nlohmann_json
)

gtest_discover_tests(compiler_test)
