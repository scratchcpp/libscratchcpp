if (LIBSCRATCHCPP_CODEGEN_BACKEND STREQUAL "cpp")
  set(CODEGEN_TEST_FUNCTIONS_TYPE OBJECT)
else()
  # LLVM resolves these helper symbols dynamically from the shared library.
  set(CODEGEN_TEST_FUNCTIONS_TYPE SHARED)
endif()

add_library(
  codegen_test_functions ${CODEGEN_TEST_FUNCTIONS_TYPE}
  testfunctions.cpp
  testfunctions.h
  testmock.h
)

target_link_libraries(
  codegen_test_functions
  scratchcpp
  scratchcpp_mocks
  GTest::gmock
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

if (LIBSCRATCHCPP_CODEGEN_BACKEND STREQUAL "cpp")
    set(CPP_EXTRA_TEST_SOURCES
      "${CMAKE_CURRENT_SOURCE_DIR}/../engine/engine_test.cpp"
      "${CMAKE_CURRENT_SOURCE_DIR}/../blocks/util.cpp"
    )
    foreach(CPP_BLOCK_EXTENSION motion looks sound event control sensing operator variable list custom)
      string(TOUPPER "${CPP_BLOCK_EXTENSION}" CPP_BLOCK_EXTENSION_UPPER)
      if (LIBSCRATCHCPP_ENABLE_${CPP_BLOCK_EXTENSION_UPPER}_BLOCKS)
        list(APPEND CPP_EXTRA_TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../blocks/${CPP_BLOCK_EXTENSION}_blocks_test.cpp")
      endif()
    endforeach()
    target_sources(codegen_test PRIVATE ${CPP_EXTRA_TEST_SOURCES})
    target_include_directories(codegen_test PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../blocks")
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
    set(CPP_GENERATED_TEST_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated_cpp_test_code")
    set(CPP_GENERATED_TEST_SOURCE "${CPP_GENERATED_TEST_DIR}/generated_cpp_test_code.cpp")
    set(CPP_GENERATED_TEST_HEADER "${CPP_GENERATED_TEST_DIR}/generated_cpp_test_code.h")
    set(CPP_GENERATED_TEST_CHUNKS)
    set(CPP_GENERATED_TEST_OUTPUTS "${CPP_GENERATED_TEST_SOURCE}" "${CPP_GENERATED_TEST_HEADER}")

    foreach(CPP_CHUNK_INDEX RANGE 0 63)
      set(CPP_CHUNK_SOURCE "${CPP_GENERATED_TEST_DIR}/generated_cpp_test_code_${CPP_CHUNK_INDEX}.cpp")
      list(APPEND CPP_GENERATED_TEST_CHUNKS "${CPP_CHUNK_SOURCE}")
      list(APPEND CPP_GENERATED_TEST_OUTPUTS "${CPP_CHUNK_SOURCE}")
    endforeach()

    get_target_property(CPP_TEST_CASE_SOURCES codegen_test SOURCES)
    list(REMOVE_ITEM CPP_TEST_CASE_SOURCES main.cpp)

    # Keep the original test sources associated with codegen_test in IDE test
    # discovery. The generator compiles them through generated translation units.
    set(CPP_TEST_GENERATOR_SOURCES)
    set(CPP_TEST_CASE_INDEX 0)
    foreach(CPP_TEST_CASE_SOURCE IN LISTS CPP_TEST_CASE_SOURCES)
      if (CPP_TEST_CASE_SOURCE MATCHES "\\.cpp$")
        get_filename_component(CPP_TEST_CASE_PATH "${CPP_TEST_CASE_SOURCE}" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
        set(CPP_TEST_WRAPPER "${CMAKE_CURRENT_BINARY_DIR}/cpp_test_script_generator_sources/test_case_${CPP_TEST_CASE_INDEX}.cpp")
        file(GENERATE OUTPUT "${CPP_TEST_WRAPPER}" CONTENT "#include \"${CPP_TEST_CASE_PATH}\"\n")
        list(APPEND CPP_TEST_GENERATOR_SOURCES "${CPP_TEST_WRAPPER}")
        math(EXPR CPP_TEST_CASE_INDEX "${CPP_TEST_CASE_INDEX} + 1")
      endif()
    endforeach()
    set_source_files_properties(${CPP_TEST_GENERATOR_SOURCES} PROPERTIES GENERATED TRUE)

    add_executable(
      cpp_test_script_generator
      cpp/testscriptgenerator.cpp
      ${CPP_TEST_GENERATOR_SOURCES}
    )
    target_compile_definitions(cpp_test_script_generator PRIVATE LIBSCRATCHCPP_CPP_TEST_GENERATOR)
    if (UNIX)
      target_compile_options(cpp_test_script_generator PRIVATE -g0)
    endif()
    target_link_libraries(
      cpp_test_script_generator
      GTest::gtest
      GTest::gmock
      scratchcpp
      scratchcpp_mocks
      codegen_test_functions
    )

    add_custom_command(
      OUTPUT ${CPP_GENERATED_TEST_OUTPUTS}
      COMMAND $<TARGET_FILE:cpp_test_script_generator> "${CPP_GENERATED_TEST_DIR}"
      DEPENDS cpp_test_script_generator
      VERBATIM
    )

    target_sources(codegen_test PRIVATE ${CPP_GENERATED_TEST_CHUNKS} "${CPP_GENERATED_TEST_SOURCE}" "${CPP_GENERATED_TEST_HEADER}")
    target_include_directories(codegen_test PRIVATE
      "${CPP_GENERATED_TEST_DIR}"
      "${CMAKE_CURRENT_SOURCE_DIR}"
      "${CMAKE_CURRENT_SOURCE_DIR}/.."
      "${CMAKE_CURRENT_SOURCE_DIR}/../../src/engine/internal/codegen/cpp"
    )
    target_compile_definitions(codegen_test PRIVATE LIBSCRATCHCPP_CPP_TEST_GENERATED_CODE)
    if (UNIX)
      target_compile_options(codegen_test PRIVATE -g0)
    endif()
    add_dependencies(codegen_test cpp_test_script_generator)

    add_subdirectory(cpp)
endif()
