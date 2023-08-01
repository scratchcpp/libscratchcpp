target_sources(scratchcpp
  PRIVATE
    libscratchcpp_global.h
    project.cpp
    project_p.cpp
    project_p.h
    scratchconfiguration.cpp
  PUBLIC
    scratchconfiguration.h
    iscratchsprite.h
    iscratchstage.h
)

add_subdirectory(blocks)
add_subdirectory(engine)
add_subdirectory(internal)
add_subdirectory(scratch)
