# blockprototype_test
add_executable(
  blockprototype_test
  blockprototype_test.cpp
)

target_link_libraries(
  blockprototype_test
  GTest::gtest_main
  scratchcpp
)

gtest_discover_tests(blockprototype_test)

# list_test
add_executable(
  list_test
  list_test.cpp
)

target_link_libraries(
  list_test
  GTest::gtest_main
  scratchcpp
)

gtest_discover_tests(list_test)

# sprite_test
add_executable(
  sprite_test
  sprite_test.cpp
)

target_link_libraries(
  sprite_test
  GTest::gtest_main
  scratchcpp
)

gtest_discover_tests(sprite_test)

# stage_test
add_executable(
  stage_test
  stage_test.cpp
)

target_link_libraries(
  stage_test
  GTest::gtest_main
  scratchcpp
)

gtest_discover_tests(stage_test)

# target_test
add_executable(
  target_test
  target_test.cpp
)

target_link_libraries(
  target_test
  GTest::gtest_main
  scratchcpp
)

gtest_discover_tests(target_test)

# value_test
add_executable(
  value_test
  value_test.cpp
)

target_link_libraries(
  value_test
  GTest::gtest_main
  scratchcpp
)

gtest_discover_tests(value_test)

# entity_test
add_executable(
  entity_test
  entity_test.cpp
)

target_link_libraries(
  entity_test
  GTest::gtest_main
  scratchcpp
)

gtest_discover_tests(entity_test)
