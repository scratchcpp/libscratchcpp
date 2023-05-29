cmake_minimum_required(VERSION 3.14)
project(my_project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_definitions(DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

set(GTEST_DIR thirdparty/googletest)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../${GTEST_DIR} ${CMAKE_CURRENT_BINARY_DIR}/${GTEST_DIR})

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

include(GoogleTest)
add_subdirectory(zip)
add_subdirectory(load_project)
add_subdirectory(compiler)
