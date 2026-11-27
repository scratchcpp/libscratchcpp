if (LIBSCRATCHCPP_CODEGEN_BACKEND STREQUAL "cpp")
  set_property(GLOBAL APPEND PROPERTY LIBSCRATCHCPP_CPP_TEST_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/engine_test.cpp"
  )
  return()
endif()

add_executable(
  engine_test
  engine_test.cpp
  engine_save_start_test.cpp
)

if (LIBSCRATCHCPP_ENABLE_EVENT_BLOCKS)
  target_compile_definitions(engine_test PRIVATE LIBSCRATCHCPP_TEST_EVENT_BLOCKS)
endif()

target_link_libraries(
  engine_test
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp
  scratchcpp_mocks
  zip
)

gtest_discover_tests(engine_test)
