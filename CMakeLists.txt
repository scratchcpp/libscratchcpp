cmake_minimum_required(VERSION 3.14)

project(libscratchcpp LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(ZIP_SRC thirdparty/zip/src)

find_package(nlohmann_json 3.9.1 REQUIRED)
find_package(utf8cpp REQUIRED)

add_library(scratchcpp SHARED)
add_subdirectory(src)

add_library(zip SHARED
	${ZIP_SRC}/zip.c
	${ZIP_SRC}/zip.h
	${ZIP_SRC}/miniz.h
)
include_directories(thirdparty/zip/src)

target_link_libraries(scratchcpp PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(scratchcpp PRIVATE utf8cpp)
target_link_libraries(scratchcpp PRIVATE zip)

target_compile_definitions(scratchcpp PRIVATE LIBSCRATCHCPP_LIBRARY)
