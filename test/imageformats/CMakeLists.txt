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

# jpeg
add_executable(
  jpeg_test
  jpeg_test.cpp
)

target_link_libraries(
  jpeg_test
  GTest::gtest_main
  scratchcpp
)

gtest_discover_tests(jpeg_test)

# png
add_executable(
  png_test
  png_test.cpp
)

target_link_libraries(
  png_test
  GTest::gtest_main
  scratchcpp
)

gtest_discover_tests(png_test)
