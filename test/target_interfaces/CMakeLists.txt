# iscratchsprite_test
add_executable(
  iscratchsprite_test
  iscratchsprite_test.cpp
  testspriteinterface.cpp
  testspriteinterface.h
)

target_link_libraries(
  iscratchsprite_test
  GTest::gtest_main
  scratchcpp
)

gtest_discover_tests(iscratchsprite_test)
