# projecturl_test
add_executable(
  projecturl_test
  projecturl_test.cpp
)

target_link_libraries(
  projecturl_test
  GTest::gtest_main
  scratchcpp
)

gtest_discover_tests(projecturl_test)

if (LIBSCRATCHCPP_NETWORK_SUPPORT)
    # downloaderfactory_test
    add_executable(
      downloaderfactory_test
      downloaderfactory_test.cpp
    )

    target_link_libraries(
      downloaderfactory_test
      GTest::gtest_main
      scratchcpp
      cpr::cpr
    )

    gtest_discover_tests(downloaderfactory_test)

    # projectdownloader_test
    add_executable(
      projectdownloader_test
      projectdownloader_test.cpp
    )

    target_link_libraries(
      projectdownloader_test
      GTest::gtest_main
      GTest::gmock_main
      scratchcpp
      scratchcpp_mocks
    )

    gtest_discover_tests(projectdownloader_test)

    # projectdownloaderfactory_test
    add_executable(
      projectdownloaderfactory_test
      projectdownloaderfactory_test.cpp
    )

    target_link_libraries(
      projectdownloaderfactory_test
      GTest::gtest_main
      scratchcpp
      cpr::cpr
    )

    gtest_discover_tests(projectdownloaderfactory_test)
endif()
