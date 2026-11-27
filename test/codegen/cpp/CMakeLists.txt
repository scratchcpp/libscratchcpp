add_executable(
  cpp_test
  main.cpp
  cppcodegenerator_test.cpp
  cppownedvalue_test.cpp
  enginesave_test.cpp
  ../../engine/engine_save_start_test.cpp
  cppexecutioncontext_test.cpp
  cppexecutablecode_test.cpp
  cppexecutablecodemock.h
)

target_compile_definitions(cpp_test PRIVATE LIBSCRATCHCPP_TEST_CPP_EXPORT)
if (LIBSCRATCHCPP_ENABLE_EVENT_BLOCKS)
  target_compile_definitions(cpp_test PRIVATE LIBSCRATCHCPP_TEST_EVENT_BLOCKS)
endif()

target_link_libraries(
  cpp_test
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp
  scratchcpp_mocks
  zip
)

gtest_discover_tests(cpp_test)
