cmake_minimum_required(VERSION 3.14)

project(libscratchcpp LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(ZIP_SRC thirdparty/zip/src)

option(LIBSCRATCHCPP_BUILD_UNIT_TESTS "Build unit tests" ON)

find_package(nlohmann_json 3.9.1 REQUIRED)
find_package(utf8cpp REQUIRED)

add_library(scratchcpp SHARED)
add_subdirectory(src)
include_directories(src) # TODO: Remove this line
include_directories(include)

target_sources(scratchcpp
  PUBLIC
    include/scratchcpp/global.h
    include/scratchcpp/project.h
    include/scratchcpp/scratchconfiguration.h
    include/scratchcpp/iengine.h
    include/scratchcpp/iextension.h
    include/scratchcpp/iblocksection.h
    include/scratchcpp/asset.h
    include/scratchcpp/costume.h
    include/scratchcpp/sound.h
    include/scratchcpp/value.h
    include/scratchcpp/entity.h
    include/scratchcpp/variable.h
    include/scratchcpp/list.h
    include/scratchcpp/inputvalue.h
    include/scratchcpp/input.h
    include/scratchcpp/field.h
    include/scratchcpp/script.h
    include/scratchcpp/broadcast.h
    include/scratchcpp/compiler.h
    include/scratchcpp/virtualmachine.h
    include/scratchcpp/blockprototype.h
    include/scratchcpp/block.h
    include/scratchcpp/istagehandler.h
    include/scratchcpp/ispritehandler.h
    include/scratchcpp/target.h
    include/scratchcpp/stage.h
    include/scratchcpp/sprite.h
    include/scratchcpp/itimer.h
    include/scratchcpp/keyevent.h
    include/scratchcpp/iimageformat.h
    include/scratchcpp/iimageformatfactory.h
    include/scratchcpp/rect.h
)

add_library(zip SHARED
	${ZIP_SRC}/zip.c
	${ZIP_SRC}/zip.h
	${ZIP_SRC}/miniz.h
)
include_directories(thirdparty/zip/src)

include_directories(thirdparty/spimpl)

include(build/FindGD.cmake)

target_link_libraries(scratchcpp PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(scratchcpp PRIVATE utf8cpp)
target_link_libraries(scratchcpp PRIVATE zip)

target_compile_definitions(scratchcpp PRIVATE LIBSCRATCHCPP_LIBRARY)

if (LIBSCRATCHCPP_BUILD_UNIT_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
