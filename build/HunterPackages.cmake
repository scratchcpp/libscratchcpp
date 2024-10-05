# https://layle.me/posts/using-llvm-with-cmake/
# HUNTER_URL is the URL to the latest source code archive on GitHub
# HUNTER_SHA1 is the hash of the downloaded archive

set(OLD_PROJECT_NAME ${PROJECT_NAME})
set(PROJECT_NAME "")

set(HUNTER_URL "https://github.com/scratchcpp/hunter/archive/ee768cdd2c027b5be346f114e726d4b0c4296de6.zip")
set(HUNTER_SHA1 "4A018750743AC656A859C99C655723EAF68EE038")

set(HUNTER_LLVM_VERSION 19.1.0)
set(HUNTER_LLVM_CMAKE_ARGS
    LLVM_ENABLE_CRASH_OVERRIDES=OFF
    LLVM_ENABLE_ASSERTIONS=ON
    LLVM_ENABLE_ZLIB=OFF
    LLVM_ENABLE_RTTI=ON
    LLVM_BUILD_EXAMPLES=OFF
    LLVM_BUILD_TOOLS=OFF
    LLVM_BUILD_LLVM_DYLIB=ON
    LLVM_INCLUDE_EXAMPLES=OFF
    LLVM_TARGETS_TO_BUILD=host
)

set(HUNTER_PACKAGES LLVM)

include(FetchContent)
message(STATUS "Fetching hunter...")
FetchContent_Declare(SetupHunter GIT_REPOSITORY https://github.com/cpp-pm/gate)
FetchContent_MakeAvailable(SetupHunter)

set(PROJECT_NAME ${OLD_PROJECT_NAME})
