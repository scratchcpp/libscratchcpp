# https://layle.me/posts/using-llvm-with-cmake/
# This is an INTERFACE target for LLVM, usage:
#   target_link_libraries(${PROJECT_NAME} <PRIVATE|PUBLIC|INTERFACE> LLVM)
# The include directories and compile definitions will be properly handled.

set(CMAKE_FOLDER_LLVM "${CMAKE_FOLDER}")
if(CMAKE_FOLDER)
    set(CMAKE_FOLDER "${CMAKE_FOLDER}/LLVM")
else()
    set(CMAKE_FOLDER "LLVM")
endif()

# Find LLVM
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Split the definitions properly (https://weliveindetail.github.io/blog/post/2017/07/17/notes-setup.html)
separate_arguments(LLVM_DEFINITIONS)

# Some diagnostics (https://stackoverflow.com/a/17666004/1806760)
message(STATUS "LLVM libraries: ${LLVM_LIBRARIES}")
message(STATUS "LLVM includes: ${LLVM_INCLUDE_DIRS}")
message(STATUS "LLVM definitions: ${LLVM_DEFINITIONS}")
message(STATUS "LLVM tools: ${LLVM_TOOLS_BINARY_DIR}")

if (NOT TARGET LLVM)
    add_library(LLVM INTERFACE)
endif()

target_include_directories(LLVM SYSTEM INTERFACE ${LLVM_INCLUDE_DIRS})
target_link_libraries(LLVM INTERFACE ${LLVM_AVAILABLE_LIBS})
target_compile_definitions(LLVM INTERFACE ${LLVM_DEFINITIONS} -DNOMINMAX)

set(CMAKE_FOLDER "${CMAKE_FOLDER_LLVM}")
unset(CMAKE_FOLDER_LLVM)

# MSVC-specific options
if(MSVC)
    # This assumes the installed LLVM was built in Release mode
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "/ZI /Od /Ob0 /DNDEBUG" CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/ZI /Od /Ob0 /DNDEBUG" CACHE STRING "" FORCE)

    if(${LLVM_USE_CRT_RELEASE} STREQUAL "MD")
        set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreadedDLL)
    elseif(${LLVM_USE_CRT_RELEASE} STREQUAL "MT")
        set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded)
    else()
        message(FATAL_ERROR "Unsupported LLVM_USE_CRT_RELEASE=${LLVM_USE_CRT_RELEASE}")
    endif()
endif()
