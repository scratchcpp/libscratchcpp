target_sources(scratchcpp
  PRIVATE
    pngimageformat.cpp
    pngimageformat.h
    pngimageformatfactory.cpp
    pngimageformatfactory.h
)

target_link_libraries(scratchcpp PRIVATE gd)
