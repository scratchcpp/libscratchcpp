cmake_minimum_required(VERSION 3.14)

project(libscratchcpp VERSION 0.14.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(LIBSCRATCHCPP_BUILD_UNIT_TESTS "Build unit tests" ON)
option(LIBSCRATCHCPP_NETWORK_SUPPORT "Support for downloading projects" ON)
option(LIBSCRATCHCPP_PRINT_LLVM_IR "Print LLVM IR of compiled Scratch scripts (for debugging)" OFF)

add_library(scratchcpp SHARED)
add_subdirectory(src)
include_directories(src) # TODO: Remove this line
include_directories(include)
install(TARGETS scratchcpp DESTINATION lib)

target_sources(scratchcpp
  PUBLIC
    include/scratchcpp/global.h
    include/scratchcpp/project.h
    include/scratchcpp/scratchconfiguration.h
    include/scratchcpp/iengine.h
    include/scratchcpp/iextension.h
    include/scratchcpp/compiler.h
    include/scratchcpp/compilercontext.h
    include/scratchcpp/compilervalue.h
    include/scratchcpp/compilerconstant.h
    include/scratchcpp/compilerlocalvariable.h
    include/scratchcpp/executablecode.h
    include/scratchcpp/executioncontext.h
    include/scratchcpp/promise.h
    include/scratchcpp/thread.h
    include/scratchcpp/asset.h
    include/scratchcpp/costume.h
    include/scratchcpp/sound.h
    include/scratchcpp/value.h
    include/scratchcpp/valuedata.h
    include/scratchcpp/value_functions.h
    include/scratchcpp/stringptr.h
    include/scratchcpp/string_functions.h
    include/scratchcpp/string_pool.h
    include/scratchcpp/entity.h
    include/scratchcpp/variable.h
    include/scratchcpp/list.h
    include/scratchcpp/inputvalue.h
    include/scratchcpp/input.h
    include/scratchcpp/field.h
    include/scratchcpp/script.h
    include/scratchcpp/broadcast.h
    include/scratchcpp/blockprototype.h
    include/scratchcpp/block.h
    include/scratchcpp/istagehandler.h
    include/scratchcpp/ispritehandler.h
    include/scratchcpp/drawable.h
    include/scratchcpp/target.h
    include/scratchcpp/stage.h
    include/scratchcpp/sprite.h
    include/scratchcpp/textbubble.h
    include/scratchcpp/itimer.h
    include/scratchcpp/istacktimer.h
    include/scratchcpp/irandomgenerator.h
    include/scratchcpp/keyevent.h
    include/scratchcpp/rect.h
    include/scratchcpp/igraphicseffect.h
    include/scratchcpp/comment.h
    include/scratchcpp/monitor.h
    include/scratchcpp/imonitorhandler.h
    include/scratchcpp/test/scriptbuilder.h
)

include(FetchContent)

# zip
include(build/zip.cmake)
target_link_libraries(scratchcpp PRIVATE zip)

# utfcpp
set(UTFCPP_SRC thirdparty/utfcpp/source)
target_include_directories(scratchcpp PUBLIC ${UTFCPP_SRC})

# cpp-unicodelib
add_subdirectory(thirdparty/cpp-unicodelib)
target_link_libraries(scratchcpp PRIVATE cpp-unicodelib)

# spimpl
include_directories(thirdparty/spimpl)

# JSON
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)
target_link_libraries(scratchcpp PRIVATE nlohmann_json::nlohmann_json)

# Audio
target_link_libraries(scratchcpp PRIVATE scratchcpp-audio)

# Network
if (LIBSCRATCHCPP_NETWORK_SUPPORT)
    FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
                         GIT_TAG 225b7454877805f089b3895260438e929bd6d123) # 09-22-2024
    FetchContent_MakeAvailable(cpr)
    target_link_libraries(scratchcpp PRIVATE cpr::cpr)
    target_compile_definitions(scratchcpp PRIVATE LIBSCRATCHCPP_NETWORK_SUPPORT)
endif()

# LLVM
include(build/HunterPackages.cmake)
include(build/LLVM.cmake)
target_link_libraries(scratchcpp PRIVATE LLVM)

if(LIBSCRATCHCPP_PRINT_LLVM_IR)
    target_compile_definitions(scratchcpp PRIVATE PRINT_LLVM_IR)
endif()

# Macros
target_compile_definitions(scratchcpp PRIVATE LIBSCRATCHCPP_LIBRARY)
target_compile_definitions(scratchcpp PRIVATE LIBSCRATCHCPP_VERSION="${PROJECT_VERSION}")
target_compile_definitions(scratchcpp PRIVATE LIBSCRATCHCPP_VERSION_MAJOR=${PROJECT_VERSION_MAJOR})
target_compile_definitions(scratchcpp PRIVATE LIBSCRATCHCPP_VERSION_MINOR=${PROJECT_VERSION_MINOR})
target_compile_definitions(scratchcpp PRIVATE LIBSCRATCHCPP_VERSION_PATCH=${PROJECT_VERSION_PATCH})

# Unit tests
if (LIBSCRATCHCPP_BUILD_UNIT_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
