# iextension_test
add_executable(
  iextension_test
  iextension_test.cpp
  testextension.cpp
  testextension.h
)

target_link_libraries(
  iextension_test
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp
  scratchcpp_mocks
)

gtest_discover_tests(iextension_test)
