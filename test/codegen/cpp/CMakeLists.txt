add_executable(
  cpp_test
  main.cpp
  cppexecutioncontext_test.cpp
  cppexecutablecode_test.cpp
  cppexecutablecodemock.h
)

target_link_libraries(
  cpp_test
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp
  scratchcpp_mocks
)

gtest_discover_tests(cpp_test)
