add_library(
  codegen_test_functions SHARED
  testfunctions.cpp
  testfunctions.h
  testmock.h
)

target_link_libraries(
  codegen_test_functions
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp
  scratchcpp_mocks
)

add_executable(
  codegen_test
  main.cpp
  codegentestutils.cpp
  codegentestutils.h
  instructionlist_test.cpp
  code_analyzer/variable_type_analysis.cpp
  code_analyzer/list_type_analysis.cpp
  code_analyzer/mixed_type_analysis.cpp
)

if (NOT LIBSCRATCHCPP_CODEGEN_BACKEND STREQUAL "cpp")
  # TODO: Move this to the executable sources
  target_sources(codegen_test PUBLIC
    codebuilder_test.cpp
    operators/equal_comparison_test.cpp
    operators/greater_than_test.cpp
    operators/less_than_test.cpp
    operators/math/add_test.cpp
    operators/math/subtract_test.cpp
    operators/math/multiply_test.cpp
    operators/math/divide_test.cpp
    operators/math/random_test.cpp
    operators/math/random_int_test.cpp
    operators/math/mod_test.cpp
    operators/math/abs_test.cpp
    operators/math/round_test.cpp
    operators/math/floor_test.cpp
    operators/math/ceil_test.cpp
    operators/math/sqrt_test.cpp
    operators/math/sin_test.cpp
    operators/math/cos_test.cpp
    operators/math/tan_test.cpp
    operators/math/asin_test.cpp
    operators/math/acos_test.cpp
    operators/math/atan_test.cpp
    operators/math/ln_test.cpp
    operators/math/log10_test.cpp
    operators/math/exp_test.cpp
    operators/math/exp10_test.cpp
    operators/logic/and_test.cpp
    operators/logic/or_test.cpp
    operators/logic/not_test.cpp
    operators/string/string_equal_case_sensitive_test.cpp
    operators/string/string_equal_case_insensitive_test.cpp
    operators/string/string_concat_test.cpp
    operators/string/string_char_test.cpp
    operators/string/string_length_test.cpp
  )
endif()

target_link_libraries(
  codegen_test
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp
  scratchcpp_mocks
  codegen_test_functions
)

gtest_discover_tests(codegen_test)

if (LIBSCRATCHCPP_CODEGEN_BACKEND STREQUAL "llvm")
    add_subdirectory(llvm)
elseif (LIBSCRATCHCPP_CODEGEN_BACKEND STREQUAL "cpp")
    add_subdirectory(cpp)
endif()
