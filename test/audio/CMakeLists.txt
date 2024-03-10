if (LIBSCRATCHCPP_AUDIO_SUPPORT)
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
endif()
