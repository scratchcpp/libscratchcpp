add_library(
  block_test_deps SHARED
  util.cpp
  util.h
)

target_link_libraries(
  block_test_deps
  GTest::gtest_main
  scratchcpp
)

# motion_blocks_test
if (LIBSCRATCHCPP_ENABLE_MOTION_BLOCKS)
    add_executable(
      motion_blocks_test
      motion_blocks_test.cpp
    )

    target_link_libraries(
      motion_blocks_test
      GTest::gtest_main
      GTest::gmock_main
      scratchcpp
      scratchcpp_mocks
    )

    gtest_discover_tests(motion_blocks_test)
endif()

# looks_blocks_test
if (LIBSCRATCHCPP_ENABLE_LOOKS_BLOCKS)
    add_executable(
      looks_blocks_test
      looks_blocks_test.cpp
    )

    target_link_libraries(
      looks_blocks_test
      GTest::gtest_main
      GTest::gmock_main
      scratchcpp
      scratchcpp_mocks
    )

    gtest_discover_tests(looks_blocks_test)
endif()

# sound_blocks_test
if (LIBSCRATCHCPP_ENABLE_SOUND_BLOCKS)
    add_executable(
      sound_blocks_test
      sound_blocks_test.cpp
    )

    target_link_libraries(
      sound_blocks_test
      GTest::gtest_main
      GTest::gmock_main
      scratchcpp
      scratchcpp_mocks
    )

    gtest_discover_tests(sound_blocks_test)
endif()

# event_blocks_test
if (LIBSCRATCHCPP_ENABLE_EVENT_BLOCKS)
    add_executable(
      event_blocks_test
      event_blocks_test.cpp
    )

    target_link_libraries(
      event_blocks_test
      GTest::gtest_main
      GTest::gmock_main
      scratchcpp
      scratchcpp_mocks
    )

    gtest_discover_tests(event_blocks_test)
endif()

# control_blocks_test
if (LIBSCRATCHCPP_ENABLE_CONTROL_BLOCKS)
    add_executable(
      control_blocks_test
      control_blocks_test.cpp
    )

    target_link_libraries(
      control_blocks_test
      GTest::gtest_main
      GTest::gmock_main
      scratchcpp
      scratchcpp_mocks
      block_test_deps
    )

    gtest_discover_tests(control_blocks_test)
endif()

# sensing_blocks_test
if (LIBSCRATCHCPP_ENABLE_SENSING_BLOCKS)
    add_executable(
      sensing_blocks_test
      sensing_blocks_test.cpp
    )

    target_link_libraries(
      sensing_blocks_test
      GTest::gtest_main
      GTest::gmock_main
      scratchcpp
      scratchcpp_mocks
    )

    gtest_discover_tests(sensing_blocks_test)
endif()

# operator_blocks_test
if (LIBSCRATCHCPP_ENABLE_OPERATOR_BLOCKS)
    add_executable(
      operator_blocks_test
      operator_blocks_test.cpp
    )

    target_link_libraries(
      operator_blocks_test
      GTest::gtest_main
      GTest::gmock_main
      scratchcpp
      scratchcpp_mocks
      block_test_deps
    )

    gtest_discover_tests(operator_blocks_test)
endif()

# variable_blocks_test
if (LIBSCRATCHCPP_ENABLE_VARIABLE_BLOCKS)
    add_executable(
      variable_blocks_test
      variable_blocks_test.cpp
    )

    target_link_libraries(
      variable_blocks_test
      GTest::gtest_main
      GTest::gmock_main
      scratchcpp
      scratchcpp_mocks
    )

    gtest_discover_tests(variable_blocks_test)
endif()

# list_blocks_test
if (LIBSCRATCHCPP_ENABLE_LIST_BLOCKS)
    add_executable(
      list_blocks_test
      list_blocks_test.cpp
    )

    target_link_libraries(
      list_blocks_test
      GTest::gtest_main
      GTest::gmock_main
      scratchcpp
      scratchcpp_mocks
    )

    gtest_discover_tests(list_blocks_test)
endif()

# custom_blocks_test
if (LIBSCRATCHCPP_ENABLE_CUSTOM_BLOCKS)
    add_executable(
      custom_blocks_test
      custom_blocks_test.cpp
    )

    target_link_libraries(
      custom_blocks_test
      GTest::gtest_main
      GTest::gmock_main
      scratchcpp
      scratchcpp_mocks
    )

    gtest_discover_tests(custom_blocks_test)
endif()
