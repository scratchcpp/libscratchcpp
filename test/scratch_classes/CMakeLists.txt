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

# drawable_test
add_executable(
  drawable_test
  drawable_test.cpp
)

target_link_libraries(
  drawable_test
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp
  scratchcpp_mocks
)

gtest_discover_tests(drawable_test)

# sprite_test
add_executable(
  sprite_test
  sprite_test.cpp
)

target_link_libraries(
  sprite_test
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp
  scratchcpp_mocks
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
  GTest::gmock_main
  scratchcpp
  scratchcpp_mocks
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
  GTest::gmock_main
  scratchcpp
  scratchcpp_mocks
)

gtest_discover_tests(target_test)

# textbubble_test
add_executable(
  textbubble_test
  textbubble_test.cpp
)

target_link_libraries(
  textbubble_test
  GTest::gmock_main
  scratchcpp
  scratchcpp_mocks
  scratchcpp
)

gtest_discover_tests(textbubble_test)

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

# broadcast_test
add_executable(
  broadcast_test
  broadcast_test.cpp
)

target_link_libraries(
  broadcast_test
  GTest::gtest_main
  scratchcpp
)

gtest_discover_tests(broadcast_test)

# input_test
add_executable(
  input_test
  input_test.cpp
)

target_link_libraries(
  input_test
  GTest::gtest_main
  scratchcpp
)

gtest_discover_tests(input_test)

# inputvalue_test
add_executable(
  inputvalue_test
  inputvalue_test.cpp
)

target_link_libraries(
  inputvalue_test
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp
  scratchcpp_mocks
)

gtest_discover_tests(inputvalue_test)

# field_test
add_executable(
  field_test
  field_test.cpp
)

target_link_libraries(
  field_test
  GTest::gtest_main
  scratchcpp
)

gtest_discover_tests(field_test)

# block_test
add_executable(
  block_test
  block_test.cpp
)

target_link_libraries(
  block_test
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp
  scratchcpp_mocks
)

gtest_discover_tests(block_test)

# variable_test
add_executable(
  variable_test
  variable_test.cpp
)

target_link_libraries(
  variable_test
  GTest::gtest_main
  scratchcpp
)

gtest_discover_tests(variable_test)

# keyevent_test
add_executable(
  keyevent_test
  keyevent_test.cpp
)

target_link_libraries(
  keyevent_test
  GTest::gtest_main
  scratchcpp
)

gtest_discover_tests(keyevent_test)

# comment_test
add_executable(
  comment_test
  comment_test.cpp
)

target_link_libraries(
  comment_test
  GTest::gtest_main
  scratchcpp
)

gtest_discover_tests(comment_test)

# monitor_test
add_executable(
  monitor_test
  monitor_test.cpp
  testextension.cpp
  testextension.h
)

target_link_libraries(
  monitor_test
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp
  scratchcpp_mocks
)

gtest_discover_tests(monitor_test)
