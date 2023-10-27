target_sources(scratchcpp
  PRIVATE
    jpegimageformat.cpp
    jpegimageformat.h
    jpegimageformatfactory.cpp
    jpegimageformatfactory.h
)

target_link_libraries(scratchcpp PRIVATE gd)
