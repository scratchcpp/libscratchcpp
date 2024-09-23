cmake_minimum_required(VERSION 3.14)

project(libscratchcpp VERSION 0.11.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(LIBSCRATCHCPP_BUILD_UNIT_TESTS "Build unit tests" ON)
option(LIBSCRATCHCPP_NETWORK_SUPPORT "Support for downloading projects" ON)
option(LIBSCRATCHCPP_COMPUTED_GOTO "Support for computed goto" ON)

if (NOT (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))
	# Computed goto not supported on anything except GCC
	set(LIBSCRATCHCPP_COMPUTED_GOTO OFF CACHE BOOL "" FORCE)
endif()

add_library(scratchcpp SHARED)
add_subdirectory(src)
include_directories(src) # TODO: Remove this line
include_directories(include)

if (LIBSCRATCHCPP_COMPUTED_GOTO)
	target_compile_definitions(scratchcpp PRIVATE ENABLE_COMPUTED_GOTO)
endif()

target_sources(scratchcpp
  PUBLIC
    include/scratchcpp/global.h
    include/scratchcpp/project.h
    include/scratchcpp/scratchconfiguration.h
    include/scratchcpp/iengine.h
    include/scratchcpp/iextension.h
    include/scratchcpp/thread.h
    include/scratchcpp/asset.h
    include/scratchcpp/costume.h
    include/scratchcpp/sound.h
    include/scratchcpp/value.h
    include/scratchcpp/valuedata.h
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
    include/scratchcpp/drawable.h
    include/scratchcpp/target.h
    include/scratchcpp/stage.h
    include/scratchcpp/sprite.h
    include/scratchcpp/itimer.h
    include/scratchcpp/keyevent.h
    include/scratchcpp/rect.h
    include/scratchcpp/igraphicseffect.h
    include/scratchcpp/comment.h
    include/scratchcpp/monitor.h
    include/scratchcpp/imonitorhandler.h
)

include(FetchContent)
set(ZIP_SRC thirdparty/zip/src)
set(UTFCPP_SRC thirdparty/utfcpp/source)

add_library(zip SHARED
	${ZIP_SRC}/zip.c
	${ZIP_SRC}/zip.h
	${ZIP_SRC}/miniz.h
)
target_include_directories(scratchcpp PUBLIC ${ZIP_SRC})

target_include_directories(scratchcpp PUBLIC ${UTFCPP_SRC})
include_directories(thirdparty/spimpl)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

target_link_libraries(scratchcpp PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(scratchcpp PRIVATE zip)
target_link_libraries(scratchcpp PRIVATE scratchcpp-audio)

if (LIBSCRATCHCPP_NETWORK_SUPPORT)
    FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
	                     GIT_TAG 3b15fa82ea74739b574d705fea44959b58142eb8) # 1.10.5
    FetchContent_MakeAvailable(cpr)
    target_link_libraries(scratchcpp PRIVATE cpr::cpr)
    target_compile_definitions(scratchcpp PRIVATE LIBSCRATCHCPP_NETWORK_SUPPORT)
endif()

target_compile_definitions(scratchcpp PRIVATE LIBSCRATCHCPP_LIBRARY)
target_compile_definitions(scratchcpp PRIVATE LIBSCRATCHCPP_VERSION="${PROJECT_VERSION}")
target_compile_definitions(scratchcpp PRIVATE LIBSCRATCHCPP_VERSION_MAJOR=${PROJECT_VERSION_MAJOR})
target_compile_definitions(scratchcpp PRIVATE LIBSCRATCHCPP_VERSION_MINOR=${PROJECT_VERSION_MINOR})
target_compile_definitions(scratchcpp PRIVATE LIBSCRATCHCPP_VERSION_PATCH=${PROJECT_VERSION_PATCH})

if (LIBSCRATCHCPP_BUILD_UNIT_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
