file(GLOB BLOCK_TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*_blocks_test.cpp")
set(ENABLED_BLOCK_TEST_SOURCES)
foreach(BLOCK_TEST_SOURCE IN LISTS BLOCK_TEST_SOURCES)
  get_filename_component(BLOCK_TEST_NAME "${BLOCK_TEST_SOURCE}" NAME_WE)
  string(REGEX REPLACE "_blocks_test$" "" BLOCK_EXTENSION "${BLOCK_TEST_NAME}")
  string(TOUPPER "${BLOCK_EXTENSION}" BLOCK_EXTENSION_UPPER)
  if (LIBSCRATCHCPP_ENABLE_${BLOCK_EXTENSION_UPPER}_BLOCKS)
    list(APPEND ENABLED_BLOCK_TEST_SOURCES "${BLOCK_TEST_SOURCE}")
  endif()
endforeach()

if (LIBSCRATCHCPP_CODEGEN_BACKEND STREQUAL "cpp")
  if (ENABLED_BLOCK_TEST_SOURCES)
    set_property(GLOBAL APPEND PROPERTY LIBSCRATCHCPP_CPP_TEST_SOURCES
      "${CMAKE_CURRENT_SOURCE_DIR}/util.cpp"
      ${ENABLED_BLOCK_TEST_SOURCES}
    )
  endif()
  return()
endif()

add_library(block_test_deps SHARED util.cpp util.h)
target_link_libraries(block_test_deps GTest::gtest_main scratchcpp)

foreach(BLOCK_TEST_SOURCE IN LISTS ENABLED_BLOCK_TEST_SOURCES)
  get_filename_component(BLOCK_TEST_NAME "${BLOCK_TEST_SOURCE}" NAME_WE)
  add_executable(${BLOCK_TEST_NAME} "${BLOCK_TEST_SOURCE}")
  target_link_libraries(${BLOCK_TEST_NAME}
    GTest::gtest_main
    GTest::gmock_main
    scratchcpp
    scratchcpp_mocks
    block_test_deps
  )
  gtest_discover_tests(${BLOCK_TEST_NAME})
endforeach()
