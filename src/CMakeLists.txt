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

add_subdirectory(blocks)
add_subdirectory(engine)
add_subdirectory(internal)
add_subdirectory(scratch)
add_subdirectory(audio)
