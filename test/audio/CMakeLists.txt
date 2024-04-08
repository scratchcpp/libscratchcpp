# audiooutput_test
add_executable(
  audiooutput_test
  audiooutput_test.cpp
)

target_link_libraries(
  audiooutput_test
  GTest::gtest_main
  scratchcpp
)

gtest_discover_tests(audiooutput_test)

if (LIBSCRATCHCPP_AUDIO_SUPPORT)
    target_compile_definitions(audiooutput_test PRIVATE LIBSCRATCHCPP_AUDIO_SUPPORT)
endif()

# audioinput_test
add_executable(
  audioinput_test
  audioinput_test.cpp
)

target_link_libraries(
  audioinput_test
  GTest::gtest_main
  scratchcpp
)

gtest_discover_tests(audioinput_test)

if (LIBSCRATCHCPP_AUDIO_SUPPORT)
    target_compile_definitions(audioinput_test PRIVATE LIBSCRATCHCPP_AUDIO_SUPPORT)
endif()

if(LIBSCRATCHCPP_AUDIO_SUPPORT)
    # audioplayer_test
    add_executable(
      audioplayer_test
      audioplayer_test.cpp
    )

    target_link_libraries(
      audioplayer_test
      GTest::gtest_main
      scratchcpp
    )

    gtest_discover_tests(audioplayer_test)
    target_compile_definitions(audioplayer_test PRIVATE LIBSCRATCHCPP_AUDIO_SUPPORT)
endif()
