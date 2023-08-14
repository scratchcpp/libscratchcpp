add_executable(
  scratchconfiguration_test
  scratchconfiguration_test.cpp
  extensionbase.cpp
  extensionbase.h
  extension1.cpp
  extension1.h
  extension2.cpp
  extension2.h
  extension3.cpp
  extension3.h
)

target_link_libraries(
  scratchconfiguration_test
  GTest::gtest_main
  scratchcpp
)

gtest_discover_tests(scratchconfiguration_test)
