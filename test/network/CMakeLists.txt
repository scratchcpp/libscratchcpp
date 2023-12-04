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
endif()
