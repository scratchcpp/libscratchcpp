option(LIBSCRATCHCPP_JPEG_SUPPORT "JPEG image support" ON)

add_subdirectory(stub)

if (LIBSCRATCHCPP_JPEG_SUPPORT)
    target_compile_definitions(scratchcpp PRIVATE JPEG_SUPPORT)
    add_subdirectory(jpeg)
endif()

target_sources(scratchcpp
  PRIVATE
    defaultimageformats.cpp
)
