target_sources(scratchcpp
  PRIVATE
    project.cpp
    project_p.cpp
    project_p.h
    scratchconfiguration.cpp
    scratchconfiguration_p.cpp
    scratchconfiguration_p.h
    rect.cpp
    rect_p.cpp
    rect_p.h
)

if (NOT LIBSCRATCHCPP_USE_LLVM)
    add_subdirectory(blocks)
endif()

add_subdirectory(engine)
add_subdirectory(internal)
add_subdirectory(scratch)
add_subdirectory(audio)

if (LIBSCRATCHCPP_USE_LLVM)
    add_subdirectory(dev)
endif()
