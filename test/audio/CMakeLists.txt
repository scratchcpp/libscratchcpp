if (LIBSCRATCHCPP_AUDIO_SUPPORT)
    # audioplayerfactory_test
    add_executable(
      audioplayerfactory_test
      audioplayerfactory_test.cpp
    )

    target_link_libraries(
      audioplayerfactory_test
      GTest::gtest_main
      scratchcpp
    )

    gtest_discover_tests(audioplayerfactory_test)
endif()
