cmake_minimum_required(VERSION 3.14)
project(libscratchcpp_test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_definitions(DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

set(GTEST_DIR thirdparty/googletest)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../${GTEST_DIR} ${CMAKE_CURRENT_BINARY_DIR}/${GTEST_DIR})

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

include(GoogleTest)

add_subdirectory(mocks)

add_subdirectory(zip)
add_subdirectory(load_project)
add_subdirectory(project)
add_subdirectory(compiler)
add_subdirectory(virtual_machine)
add_subdirectory(scratch_classes)
add_subdirectory(target_interfaces)
add_subdirectory(blocks)
add_subdirectory(scratchconfiguration)
add_subdirectory(assets)
add_subdirectory(script)
add_subdirectory(extensions)
add_subdirectory(engine)
add_subdirectory(clock)
add_subdirectory(timer)
add_subdirectory(randomgenerator)
add_subdirectory(imageformats)
add_subdirectory(rect)
