option(LIBSCRATCHCPP_JPEG_SUPPORT "JPEG image support" ON)
option(LIBSCRATCHCPP_PNG_SUPPORT "PNG image support" ON)

add_subdirectory(stub)

if (LIBSCRATCHCPP_JPEG_SUPPORT)
    target_compile_definitions(scratchcpp PRIVATE JPEG_SUPPORT)
    add_subdirectory(jpeg)
endif()

if (LIBSCRATCHCPP_PNG_SUPPORT)
    target_compile_definitions(scratchcpp PRIVATE PNG_SUPPORT)
    add_subdirectory(png)
endif()

target_sources(scratchcpp
  PRIVATE
    defaultimageformats.cpp
)
