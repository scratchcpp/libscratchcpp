# stub
add_executable(
  imageformatstub_test
  imageformatstub_test.cpp
)

target_link_libraries(
  imageformatstub_test
  GTest::gtest_main
  scratchcpp
)

gtest_discover_tests(imageformatstub_test)
