target_sources(scratchcpp
  PRIVATE
    libscratchcpp_global.h
    scratchproject.cpp
    scratchconfiguration.cpp
  PUBLIC
    scratchproject.h
    scratchconfiguration.h
    iscratchsprite.h
    iscratchstage.h
)

add_subdirectory(blocks)
add_subdirectory(engine)
add_subdirectory(internal)
add_subdirectory(scratch)
